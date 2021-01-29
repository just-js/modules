#include "fs.h"

void just::fs::Unlink(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value fname(isolate, args[0]);
  args.GetReturnValue().Set(Integer::New(isolate, unlink(*fname)));
}

void just::fs::Realpath(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  String::Utf8Value fname(isolate, args[0]);
  Local<ArrayBuffer> buf = args[1].As<ArrayBuffer>();
  std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
  char* newpath = (char*)backing->Data();
  newpath = realpath(*fname, newpath);
  if (newpath == NULL) {
    args.GetReturnValue().Set(Integer::New(isolate, -1));
  }
  args.GetReturnValue().Set(Integer::New(isolate, 0));
}

void just::fs::Utime(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  String::Utf8Value fname(isolate, args[0]);
  // from http://rosettacode.org/wiki/File/Modification_Time#C
  struct utimbuf new_times;
  struct stat info;
  stat(*fname, &info);
  //time_t mtime = info.st_mtime;
  new_times.actime = info.st_atime; /* keep atime unchanged */
  new_times.modtime = time(NULL); /* set mtime to current time */
  args.GetReturnValue().Set(Integer::New(isolate, utime(*fname, &new_times)));
}

void just::fs::Symlink(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value target(isolate, args[0]);
  String::Utf8Value linkpath(isolate, args[1]);
  args.GetReturnValue().Set(Integer::New(isolate, symlink(*target, *linkpath)));
}

void just::fs::Mknod(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value target(isolate, args[0]);
  unsigned int type = Local<Integer>::Cast(args[1])->Value();
  unsigned int mode = Local<Integer>::Cast(args[2])->Value();
  int major = Local<Integer>::Cast(args[3])->Value();
  int minor = Local<Integer>::Cast(args[4])->Value();
  dev_t dev = makedev(major, minor);
  args.GetReturnValue().Set(Integer::New(isolate, mknod(*target, type | mode, dev)));
}

void just::fs::Mount(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value source(isolate, args[0]);
  String::Utf8Value target(isolate, args[1]);
  String::Utf8Value fstype(isolate, args[2]);
  Local<BigInt> address64 = Local<BigInt>::Cast(args[3]);
  String::Utf8Value opts(isolate, args[4]);
  unsigned long flags = reinterpret_cast<unsigned long>(address64->Uint64Value());
  args.GetReturnValue().Set(Integer::New(isolate, mount(*source, *target, *fstype, flags, *opts)));
}

void just::fs::Umount(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value target(isolate, args[0]);
  int flags = 0;
  if (args.Length() > 1) {
    flags = Local<Integer>::Cast(args[1])->Value();
  }
  args.GetReturnValue().Set(Integer::New(isolate, umount2(*target, flags)));
}

void just::fs::Open(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  String::Utf8Value fname(isolate, args[0]);
  int argc = args.Length();
  int flags = O_RDONLY;
  if (argc > 1) {
    flags = args[1]->Int32Value(context).ToChecked();
  }
  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  if (argc > 2) {
    mode = args[2]->Int32Value(context).ToChecked();
  }
  args.GetReturnValue().Set(Integer::New(isolate, open(*fname, flags, mode)));
}

void just::fs::Chmod(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int fd = Local<Integer>::Cast(args[0])->Value();
  int mode = Local<Integer>::Cast(args[1])->Value();
  //int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  args.GetReturnValue().Set(Integer::New(isolate, fchmod(fd, mode)));
}

void just::fs::Chown(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  int fd = Local<Integer>::Cast(args[0])->Value();
  int uid = Local<Integer>::Cast(args[1])->Value();
  int gid = Local<Integer>::Cast(args[2])->Value();
  args.GetReturnValue().Set(Integer::New(isolate, fchown(fd, uid, gid)));
}

void just::fs::Ioctl(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  int flags = args[1]->Int32Value(context).ToChecked();
  if (args.Length() > 2) {
    Local<ArrayBuffer> buf = args[2].As<ArrayBuffer>();
    std::shared_ptr<BackingStore> backing = buf->GetBackingStore();
    args.GetReturnValue().Set(Integer::New(isolate, ioctl(fd, flags, backing->Data())));
    return;
  }
  args.GetReturnValue().Set(Integer::New(isolate, ioctl(fd, flags)));
}

void just::fs::Ftruncate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  off_t length = args[1]->Uint32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, ftruncate(fd, length)));
}

void just::fs::Fstat(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  int fd = args[0]->Int32Value(context).ToChecked();
  Local<BigUint64Array> answer = args[1].As<BigUint64Array>();
  Local<ArrayBuffer> ab = answer->Buffer();
  std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
  uint64_t *fields = static_cast<uint64_t *>(backing->Data());
  struct stat s;
  int rc = fstat(fd, &s);
  if (rc == 0) {
    fields[0] = s.st_dev;
    fields[1] = s.st_mode;
    fields[2] = s.st_nlink;
    fields[3] = s.st_uid;
    fields[4] = s.st_gid;
    fields[5] = s.st_rdev;
    fields[6] = s.st_ino;
    fields[7] = s.st_size;
    fields[8] = s.st_blksize;
    fields[9] = s.st_blocks;
    //fields[10] = s.st_flags;
    //fields[11] = s.st_gen;
    fields[12] = s.st_atim.tv_sec;
    fields[13] = s.st_atim.tv_nsec;
    fields[14] = s.st_mtim.tv_sec;
    fields[15] = s.st_mtim.tv_nsec;
    fields[16] = s.st_ctim.tv_sec;
    fields[17] = s.st_ctim.tv_nsec;
    args.GetReturnValue().Set(Integer::New(isolate, 0));
  }
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}

void just::fs::Chdir(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value path(isolate, args[0]);
  args.GetReturnValue().Set(Integer::New(isolate, chdir(*path)));
}

void just::fs::Rmdir(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value path(isolate, args[0]);
  int rc = rmdir(*path);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}

void just::fs::Rename(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  String::Utf8Value source(isolate, args[0]);
  String::Utf8Value dest(isolate, args[1]);
  args.GetReturnValue().Set(Integer::New(isolate, rename(*source, *dest)));
}

void just::fs::Mkdir(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  String::Utf8Value path(isolate, args[0]);
  int mode = S_IRWXO | S_IRWXG | S_IRWXU;
  int argc = args.Length();
  if (argc > 1) {
    mode = args[1]->Int32Value(context).ToChecked();
  }
  int rc = mkdir(*path, mode);
  args.GetReturnValue().Set(Integer::New(isolate, rc));
}

void just::fs::Lseek(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  String::Utf8Value path(isolate, args[0]);
  int fd = args[0]->Int32Value(context).ToChecked();
  int off = args[1]->Int32Value(context).ToChecked();
  args.GetReturnValue().Set(Integer::New(isolate, lseek(fd, off, SEEK_SET)));
}

void just::fs::Readdir(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  String::Utf8Value path(isolate, args[0]);
  Local<Array> answer = args[1].As<Array>();
  DIR* directory = opendir(*path);
  if (directory == NULL) {
    args.GetReturnValue().Set(Null(isolate));
    return;
  }
  dirent* entry = readdir(directory);
  if (entry == NULL) {
    args.GetReturnValue().Set(Null(isolate));
    return;
  }
  int i = 0;
  while (entry) {
    Local<Object> o = Object::New(isolate);
    o->Set(context, String::NewFromUtf8Literal(isolate, "name", 
      NewStringType::kNormal), 
      String::NewFromUtf8(isolate, entry->d_name).ToLocalChecked()).Check();
    o->Set(context, String::NewFromUtf8Literal(isolate, "type", 
      NewStringType::kNormal), 
      Integer::New(isolate, entry->d_type)).Check();
    o->Set(context, String::NewFromUtf8Literal(isolate, "ino", 
      NewStringType::kNormal), 
      Integer::New(isolate, entry->d_ino)).Check();
    o->Set(context, String::NewFromUtf8Literal(isolate, "off", 
      NewStringType::kNormal), 
      Integer::New(isolate, entry->d_off)).Check();
    o->Set(context, String::NewFromUtf8Literal(isolate, "reclen", 
      NewStringType::kNormal), 
        Integer::New(isolate, entry->d_reclen)).Check();
    answer->Set(context, i++, o).Check();
    entry = readdir(directory);
    if (i == 1023) break;
  }
  closedir(directory);
  args.GetReturnValue().Set(answer);
}

void just::fs::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> fs = ObjectTemplate::New(isolate);
  SET_METHOD(isolate, fs, "open", just::fs::Open);
  SET_METHOD(isolate, fs, "unlink", just::fs::Unlink);
  SET_METHOD(isolate, fs, "symlink", just::fs::Symlink);
  SET_METHOD(isolate, fs, "ioctl", just::fs::Ioctl);
  SET_METHOD(isolate, fs, "rmdir", just::fs::Rmdir);
  SET_METHOD(isolate, fs, "ftruncate", just::fs::Ftruncate);
  SET_METHOD(isolate, fs, "rename", just::fs::Rename);
  SET_METHOD(isolate, fs, "mkdir", just::fs::Mkdir);
  SET_METHOD(isolate, fs, "fstat", just::fs::Fstat);
  SET_METHOD(isolate, fs, "lseek", just::fs::Lseek);
  SET_METHOD(isolate, fs, "readdir", just::fs::Readdir);
  SET_METHOD(isolate, fs, "chdir", just::fs::Chdir);
  SET_METHOD(isolate, fs, "mount", just::fs::Mount);
  SET_METHOD(isolate, fs, "umount", just::fs::Umount);
  SET_METHOD(isolate, fs, "mknod", just::fs::Mknod);
  SET_METHOD(isolate, fs, "realpath", just::fs::Realpath);
  SET_METHOD(isolate, fs, "utime", just::fs::Utime);

  SET_METHOD(isolate, fs, "chmod", just::fs::Chmod);
  SET_METHOD(isolate, fs, "chown", just::fs::Chown);
  // todo: move fcntl here

  SET_VALUE(isolate, fs, "O_RDONLY", Integer::New(isolate, O_RDONLY));
  SET_VALUE(isolate, fs, "O_WRONLY", Integer::New(isolate, O_WRONLY));
  SET_VALUE(isolate, fs, "O_RDWR", Integer::New(isolate, O_RDWR));
  SET_VALUE(isolate, fs, "O_CREAT", Integer::New(isolate, O_CREAT));
  SET_VALUE(isolate, fs, "O_EXCL", Integer::New(isolate, O_EXCL));
  SET_VALUE(isolate, fs, "O_APPEND", Integer::New(isolate, O_APPEND));
  SET_VALUE(isolate, fs, "O_SYNC", Integer::New(isolate, O_SYNC));
  SET_VALUE(isolate, fs, "O_TRUNC", Integer::New(isolate, O_TRUNC));

  SET_VALUE(isolate, fs, "S_IRUSR", Integer::New(isolate, S_IRUSR));
  SET_VALUE(isolate, fs, "S_IWUSR", Integer::New(isolate, S_IWUSR));
  SET_VALUE(isolate, fs, "S_IXUSR", Integer::New(isolate, S_IXUSR));
  SET_VALUE(isolate, fs, "S_IRGRP", Integer::New(isolate, S_IRGRP));
  SET_VALUE(isolate, fs, "S_IWGRP", Integer::New(isolate, S_IWGRP));
  SET_VALUE(isolate, fs, "S_IXGRP", Integer::New(isolate, S_IXGRP));
  SET_VALUE(isolate, fs, "S_IROTH", Integer::New(isolate, S_IROTH));
  SET_VALUE(isolate, fs, "S_IWOTH", Integer::New(isolate, S_IWOTH));
  SET_VALUE(isolate, fs, "S_IXOTH", Integer::New(isolate, S_IXOTH));
  SET_VALUE(isolate, fs, "S_IRWXO", Integer::New(isolate, S_IRWXO));
  SET_VALUE(isolate, fs, "S_IRWXG", Integer::New(isolate, S_IRWXG));
  SET_VALUE(isolate, fs, "S_IRWXU", Integer::New(isolate, S_IRWXU));

  SET_VALUE(isolate, fs, "DT_BLK", Integer::New(isolate, DT_BLK));
  SET_VALUE(isolate, fs, "DT_CHR", Integer::New(isolate, DT_CHR));
  SET_VALUE(isolate, fs, "DT_DIR", Integer::New(isolate, DT_DIR));
  SET_VALUE(isolate, fs, "DT_FIFO", Integer::New(isolate, DT_FIFO));
  SET_VALUE(isolate, fs, "DT_LNK", Integer::New(isolate, DT_LNK));
  SET_VALUE(isolate, fs, "DT_REG", Integer::New(isolate, DT_REG));
  SET_VALUE(isolate, fs, "DT_SOCK", Integer::New(isolate, DT_SOCK));
  SET_VALUE(isolate, fs, "DT_UNKNOWN", Integer::New(isolate, DT_UNKNOWN));

  SET_VALUE(isolate, fs, "SEEK_SET", Integer::New(isolate, SEEK_SET));
  SET_VALUE(isolate, fs, "SEEK_CUR", Integer::New(isolate, SEEK_CUR));
  SET_VALUE(isolate, fs, "SEEK_END", Integer::New(isolate, SEEK_END));

  SET_VALUE(isolate, fs, "EPERM", Integer::New(isolate, EPERM));
  SET_VALUE(isolate, fs, "ENOENT", Integer::New(isolate, ENOENT));
  SET_VALUE(isolate, fs, "ESRCH", Integer::New(isolate, ESRCH));
  SET_VALUE(isolate, fs, "EINTR", Integer::New(isolate, EINTR));
  SET_VALUE(isolate, fs, "EIO", Integer::New(isolate, EIO));
  SET_VALUE(isolate, fs, "ENXIO", Integer::New(isolate, ENXIO));
  SET_VALUE(isolate, fs, "E2BIG", Integer::New(isolate, E2BIG));
  SET_VALUE(isolate, fs, "ENOEXEC", Integer::New(isolate, ENOEXEC));
  SET_VALUE(isolate, fs, "EBADF", Integer::New(isolate, EBADF));
  SET_VALUE(isolate, fs, "ECHILD", Integer::New(isolate, ECHILD));
  SET_VALUE(isolate, fs, "EAGAIN", Integer::New(isolate, EAGAIN));
  SET_VALUE(isolate, fs, "ENOMEM", Integer::New(isolate, ENOMEM));
  SET_VALUE(isolate, fs, "EACCES", Integer::New(isolate, EACCES));
  SET_VALUE(isolate, fs, "EFAULT", Integer::New(isolate, EFAULT));
  SET_VALUE(isolate, fs, "ENOTBLK", Integer::New(isolate, ENOTBLK));
  SET_VALUE(isolate, fs, "EBUSY", Integer::New(isolate, EBUSY));
  SET_VALUE(isolate, fs, "EEXIST", Integer::New(isolate, EEXIST));
  SET_VALUE(isolate, fs, "EXDEV", Integer::New(isolate, EXDEV));
  SET_VALUE(isolate, fs, "ENODEV", Integer::New(isolate, ENODEV));
  SET_VALUE(isolate, fs, "ENOTDIR", Integer::New(isolate, ENOTDIR));
  SET_VALUE(isolate, fs, "EISDIR", Integer::New(isolate, EISDIR));
  SET_VALUE(isolate, fs, "EINVAL", Integer::New(isolate, EINVAL));
  SET_VALUE(isolate, fs, "ENFILE", Integer::New(isolate, ENFILE));
  //SET_VALUE(isolate, fs, "ENFILE", Integer::New(isolate, ENFILE));
  SET_VALUE(isolate, fs, "ENOTTY", Integer::New(isolate, ENOTTY));
  SET_VALUE(isolate, fs, "ETXTBSY", Integer::New(isolate, ETXTBSY));
  SET_VALUE(isolate, fs, "EFBIG", Integer::New(isolate, EFBIG));
  SET_VALUE(isolate, fs, "ENOSPC", Integer::New(isolate, ENOSPC));
  SET_VALUE(isolate, fs, "ESPIPE", Integer::New(isolate, ESPIPE));
  SET_VALUE(isolate, fs, "EROFS", Integer::New(isolate, EROFS));
  SET_VALUE(isolate, fs, "EMLINK", Integer::New(isolate, EMLINK));
  SET_VALUE(isolate, fs, "EPIPE", Integer::New(isolate, EPIPE));
  SET_VALUE(isolate, fs, "EDOM", Integer::New(isolate, EDOM));
  SET_VALUE(isolate, fs, "ERANGE", Integer::New(isolate, ERANGE));

  SET_VALUE(isolate, fs, "S_IFMT", Integer::New(isolate, S_IFMT));
  SET_VALUE(isolate, fs, "S_IFSOCK", Integer::New(isolate, S_IFSOCK));
  SET_VALUE(isolate, fs, "S_IFLNK", Integer::New(isolate, S_IFLNK));
  SET_VALUE(isolate, fs, "S_IFREG", Integer::New(isolate, S_IFREG));
  SET_VALUE(isolate, fs, "S_IFBLK", Integer::New(isolate, S_IFBLK));
  SET_VALUE(isolate, fs, "S_IFDIR", Integer::New(isolate, S_IFDIR));
  SET_VALUE(isolate, fs, "S_IFCHR", Integer::New(isolate, S_IFCHR));
  SET_VALUE(isolate, fs, "S_IFIFO", Integer::New(isolate, S_IFIFO));

  SET_MODULE(isolate, target, "fs", fs);
}
