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

void kvm_cmd_run_init(struct kvm *kvm, int kernel_fd, int initrd_fd, int fs_fd) {
  kvm->cfg.using_rootfs = 0;
  kvm->cfg.fd_kernel = kernel_fd;
  if (initrd_fd > 0) {
    kvm->cfg.fd_initrd = initrd_fd;
  }
  kvm->cfg.image_count = 0;
  if (fs_fd > -1) {
    kvm->cfg.image_count = 1;
    kvm->cfg.disk_image[0].filename = "/dev/shm/rootfs";
    kvm->cfg.disk_image[0].fd = fs_fd;
  }
  kvm->nr_disks = kvm->cfg.image_count;
  kvm->cfg.nrcpus = 1;
  kvm->cfg.ram_size = 64;
	kvm->cfg.ram_size <<= MB_SHIFT;
	kvm->cfg.console = "serial";
  kvm->cfg.active_console  = CONSOLE_8250;
	kvm->cfg.host_ip = DEFAULT_HOST_ADDR;
	kvm->cfg.guest_ip = DEFAULT_GUEST_ADDR;
	kvm->cfg.guest_mac = DEFAULT_GUEST_MAC;
  kvm->cfg.host_mac = DEFAULT_HOST_MAC;
  //kvm->cfg.network = "mode=tap,tapif=tap0";
  kvm->cfg.network = "none";
  kvm->cfg.dev = "/dev/kvm";
  kvm->cfg.balloon = 0;
  kvm->cfg.vnc = 0;
  kvm->cfg.gtk = 0;
  kvm->cfg.sdl = 0;
  kvm->cfg.virtio_rng = 0;
  kvm->cfg.no_net = 1;
  kvm->cfg.no_dhcp = 1;
  kvm->cfg.guest_name = "foo";
	kvm->cfg.real_cmdline = "noapic noacpi pci=conf1 reboot=k panic=1 i8042.direct=1 i8042.dumbkbd=1 i8042.nopnp=1 earlyprintk=serial i8042.noaux=1 console=ttyS0 root=/dev/vda rw panic=-1 selinux=0 nomodules random.trust_cpu=on quiet audit=0";
}

void just::jkvm::CreateSync(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
	kvm__set_dir("%s/%s", HOME_DIR, KVM_PID_FILE_PATH);
  int kernel_fd = Local<Integer>::Cast(args[0])->Value();
  int initrd_fd = Local<Integer>::Cast(args[1])->Value();
  int fs_fd = Local<Integer>::Cast(args[2])->Value();
	int ret = -EFAULT;
	struct kvm *kvm = kvm__new();
  args.GetReturnValue().Set(Integer::New(isolate, 1));
	if (IS_ERR(kvm)) return;
	kvm_cmd_run_init(kvm, kernel_fd, initrd_fd, fs_fd);
	if (IS_ERR(kvm)) return;
	if (init_list__init(kvm) < 0) die("Initialisation failed");
	if (IS_ERR(kvm)) return;
	for (int i = 0; i < kvm->nrcpus; i++) {
    int r = pthread_create(&kvm->cpus[i]->thread, NULL, kvm_cpu_thread, kvm->cpus[i]);
		if (r != 0) {
      return;
    };
	}
	if (pthread_join(kvm->cpus[0]->thread, NULL) != 0) die("unable to join with vcpu 0");
  pci__exit(kvm);
	init_list__exit(kvm);
	ret = kvm_cpu__exit(kvm);
	//virtio_blk__exit(kvm);
	//virtio_net__exit(kvm);
  args.GetReturnValue().Set(Integer::New(isolate, ret));
}

void just::jkvm::Create(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  int kernel_fd = Local<Integer>::Cast(args[1])->Value();
  int initrd_fd = Local<Integer>::Cast(args[2])->Value();
  int fs_fd = Local<Integer>::Cast(args[3])->Value();
  Local<Context> context = isolate->GetCurrentContext();
	kvm__set_dir("%s/%s", HOME_DIR, KVM_PID_FILE_PATH);
	//int ret = -EFAULT;
	struct kvm *kvm = kvm__new();
  Local<Array> threads = Array::New(isolate);
  args.GetReturnValue().Set(threads);
	if (IS_ERR(kvm)) return;
	kvm_cmd_run_init(kvm, kernel_fd, initrd_fd, fs_fd);
	if (IS_ERR(kvm)) return;
	if (init_list__init(kvm) < 0) die("Initialisation failed");
	if (IS_ERR(kvm)) return;
	for (int i = 0; i < kvm->nrcpus; i++) {
    if (pthread_create(&kvm->cpus[i]->thread, NULL, kvm_cpu_thread, kvm->cpus[i]) != 0) return;
    threads->Set(context, i, BigInt::New(isolate, kvm->cpus[i]->thread)).Check();
	}
  ab->SetAlignedPointerInInternalField(1, kvm);
}

void just::jkvm::Destroy(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<ArrayBuffer> ab = args[0].As<ArrayBuffer>();
  struct kvm* kvm = (struct kvm*)ab->GetAlignedPointerFromInternalField(1);
	init_list__exit(kvm);
  //pci__exit(kvm);
	int ret = kvm_cpu__exit(kvm);
	//virtio_blk__exit(kvm);
	//virtio_net__exit(kvm);
  //free(kvm);
  args.GetReturnValue().Set(Integer::New(isolate, ret));
}

void just::jkvm::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, module, "create", Create);
  SET_METHOD(isolate, module, "createSync", CreateSync);
  SET_METHOD(isolate, module, "destroy", Destroy);
  SET_MODULE(isolate, target, "jkvm", module);
}
