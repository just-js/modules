#include "jkvm.h"

static void *kvm_cpu_thread(void *arg) {
	char name[16];
	current_kvm_cpu = (kvm_cpu*)arg;
	sprintf(name, "kvm-vcpu-%lu", current_kvm_cpu->cpu_id);
	kvm__set_thread_name(name);
	if (kvm_cpu__start(current_kvm_cpu)) {
    fprintf(stderr, "KVM exit reason: %u (\"%s\")\n",
      current_kvm_cpu->kvm_run->exit_reason,
      kvm_exit_reasons[current_kvm_cpu->kvm_run->exit_reason]);
    if (current_kvm_cpu->kvm_run->exit_reason == KVM_EXIT_UNKNOWN)
      fprintf(stderr, "KVM exit code: 0x%llu\n",
        (unsigned long long)current_kvm_cpu->kvm_run->hw.hardware_exit_reason);
    kvm_cpu__set_debug_fd(STDOUT_FILENO);
    kvm_cpu__show_registers(current_kvm_cpu);
    kvm_cpu__show_code(current_kvm_cpu);
    kvm_cpu__show_page_tables(current_kvm_cpu);
    return (void *) (intptr_t) 1;
  }
	return (void *) (intptr_t) 0;
}

void kvm_cmd_run_init(struct kvm *kvm, int kernel_fd, int fs_fd, char* name, char* cmdline, char* mac, char* hostmac, int tapfd, int cpus, int mem, int fs_fd2) {
  kvm->cfg.using_rootfs = 0;
  kvm->cfg.fd_kernel = kernel_fd;
  kvm->cfg.image_count = 1;
  kvm->cfg.disk_image[0].readonly = false;
  kvm->cfg.disk_image[0].direct = false;
  kvm->cfg.disk_image[0].fd = fs_fd;
  if (fs_fd2 > -1) {
    kvm->cfg.image_count = 2;
    kvm->cfg.disk_image[1].readonly = false;
    kvm->cfg.disk_image[1].direct = false;
    kvm->cfg.disk_image[1].fd = fs_fd2;
  }
  kvm->nr_disks = kvm->cfg.image_count;
  kvm->cfg.nrcpus = cpus;
  kvm->cfg.ram_size = mem;
	kvm->cfg.ram_size <<= MB_SHIFT;
	//kvm->cfg.console = "serial";
	//kvm->cfg.console = "virtio";
  //kvm->cfg.active_console  = CONSOLE_VIRTIO;
  kvm->cfg.active_console  = CONSOLE_8250;
  kvm->cfg.dev = "/dev/kvm";
  kvm->cfg.balloon = 0;
  kvm->cfg.vnc = 0;
  kvm->cfg.gtk = 0;
  kvm->cfg.sdl = 0;
  kvm->cfg.virtio_rng = 1;
  kvm->cfg.no_net = 0;
  kvm->cfg.no_dhcp = 1;
  kvm->cfg.num_net_devices = 1;

	struct virtio_net_params* p = (struct virtio_net_params*)calloc(1, sizeof(struct virtio_net_params));
  p->script = "none";
  p->downscript = "none";
  p->mode = NET_MODE_TAP;
  p->kvm = kvm;
  p->vhost = 0;
	str_to_mac(mac, p->guest_mac);
	str_to_mac(hostmac, p->host_mac);
  p->mq = 8;
  p->fd = tapfd;
  p->tapif = name;

	kvm->cfg.net_params = p;
  kvm->cfg.guest_name = name;
	kvm->cfg.real_cmdline = cmdline;
}

void just::jkvm::Create(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> tpl = ObjectTemplate::New(isolate);
  tpl->SetInternalFieldCount(1);
  Local<Object> vm = tpl->NewInstance(context).ToLocalChecked();


  int kernel_fd = Local<Integer>::Cast(args[0])->Value();
  int fs_fd = Local<Integer>::Cast(args[1])->Value();
  String::Utf8Value name(isolate, args[2]);
  String::Utf8Value cmdline(isolate, args[3]);
  String::Utf8Value mac(isolate, args[4]);
  String::Utf8Value hostmac(isolate, args[5]);
  int tapfd = Local<Integer>::Cast(args[6])->Value();
  int cpus = Local<Integer>::Cast(args[7])->Value();
  int mem = Local<Integer>::Cast(args[8])->Value();
  int fs_fd2 = -1;
  if (args.Length() > 9) {
    String::Utf8Value homeDir(isolate, args[9]);
    kvm__set_dir("%s/%s", *homeDir, "");
  } else {
    kvm__set_dir("%s/%s", HOME_DIR, KVM_PID_FILE_PATH);
  }
  if (args.Length() > 10) {
    fs_fd2 = Local<Integer>::Cast(args[10])->Value();
  }

	int ret = -EFAULT;
	struct kvm *kvm = kvm__new();

	if (IS_ERR(kvm)) {
    return;
  }
	kvm_cmd_run_init(kvm, kernel_fd, fs_fd, *name, *cmdline, *mac, *hostmac, tapfd, cpus, mem, fs_fd2);
	if (IS_ERR(kvm)) {
    return;
  }
	if (init_list__init(kvm) < 0) {
    return;
  }
	if (IS_ERR(kvm)) {
    return;
  }
  vm->SetAlignedPointerInInternalField(0, kvm);
  args.GetReturnValue().Set(vm);
}

void just::jkvm::Start(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> vm = args[0].As<Object>();
  struct kvm* kvm = (struct kvm*)vm->GetAlignedPointerFromInternalField(0);
  Local<Array> threads = Array::New(isolate);
	for (int i = 0; i < kvm->nrcpus; i++) {
    int r = pthread_create(&kvm->cpus[i]->thread, NULL, kvm_cpu_thread, kvm->cpus[i]);
		if (r != 0) {
      fprintf(stderr, "could not create thread\n");
      return;
    };
    threads->Set(context, i, BigInt::New(isolate, kvm->cpus[i]->thread)).Check();
	}
  args.GetReturnValue().Set(threads);
}

void just::jkvm::Destroy(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> vm = args[0].As<Object>();
  struct kvm* kvm = (struct kvm*)vm->GetAlignedPointerFromInternalField(0);
  pci__exit(kvm);
	init_list__exit(kvm);
	int ret = kvm_cpu__exit(kvm);
	//virtio_blk__exit(kvm);
	//virtio_net__exit(kvm);
  //free(kvm);
  args.GetReturnValue().Set(Integer::New(isolate, ret));
}

void just::jkvm::State(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> vm = args[0].As<Object>();
  struct kvm* kvm = (struct kvm*)vm->GetAlignedPointerFromInternalField(0);
  args.GetReturnValue().Set(Integer::New(isolate, kvm->vm_state));
}

void just::jkvm::Stop(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> vm = args[0].As<Object>();
  struct kvm* kvm = (struct kvm*)vm->GetAlignedPointerFromInternalField(0);
  kvm__reboot(kvm);
}

void just::jkvm::Pause(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> vm = args[0].As<Object>();
  struct kvm* kvm = (struct kvm*)vm->GetAlignedPointerFromInternalField(0);
  if (kvm->vm_state == KVM_VMSTATE_RUNNING) {
		kvm->vm_state = KVM_VMSTATE_PAUSED;
		ioctl(kvm->vm_fd, KVM_KVMCLOCK_CTRL);
		kvm__pause(kvm);
  } else {
		kvm->vm_state = KVM_VMSTATE_RUNNING;
		kvm__continue(kvm);
  }
}

void just::jkvm::Debug(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> vm = args[0].As<Object>();
  struct kvm* kvm = (struct kvm*)vm->GetAlignedPointerFromInternalField(0);
	struct kvm_cpu *cpu = kvm->cpus[0];
  kvm_cpu__set_debug_fd(1);
	int fd = kvm_cpu__get_debug_fd();
	if (!cpu || cpu->needs_nmi)
		return;
	dprintf(fd, "\n #\n # vCPU #%ld's dump:\n #\n", cpu->cpu_id);
	kvm_cpu__show_registers(cpu);
	kvm_cpu__show_code(cpu);
	kvm_cpu__show_page_tables(cpu);
	fflush(stdout);
}

void just::jkvm::Balloon(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> vm = args[0].As<Object>();
  struct kvm* kvm = (struct kvm*)vm->GetAlignedPointerFromInternalField(0);
  int size = 64;
  //handle_mem(kvm, 0, 1, 4, (uint8_t*)&size);
}

void just::jkvm::Attach(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> vm = args[0].As<Object>();
  struct kvm* kvm = (struct kvm*)vm->GetAlignedPointerFromInternalField(0);
  int fs_fd = Local<Integer>::Cast(args[1])->Value();
  int current = kvm->cfg.image_count;
  kvm->cfg.image_count++;
  kvm->cfg.disk_image[current].readonly = false;
  kvm->cfg.disk_image[current].direct = false;
  kvm->cfg.disk_image[current].fd = fs_fd;
  kvm->cfg.disk_image[current].wwpn = 0;
  fprintf(stderr, "%i\n", current);
  kvm->nr_disks = kvm->cfg.image_count;
}

void just::jkvm::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "create", Create);
  SET_METHOD(isolate, module, "start", Start);
  SET_METHOD(isolate, module, "destroy", Destroy);

  SET_METHOD(isolate, module, "state", State);
  SET_METHOD(isolate, module, "pause", Pause);
  SET_METHOD(isolate, module, "stop", Stop);
  SET_METHOD(isolate, module, "debug", Debug);
  SET_METHOD(isolate, module, "balloon", Balloon);
  SET_METHOD(isolate, module, "attach", Attach);

  SET_MODULE(isolate, target, "jkvm", module);
}
