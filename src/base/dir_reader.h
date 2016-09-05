#ifndef SPARROW_FILES_DIR_READER_H_
#define SPARROW_FILES_DIR_READER_H_
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <glog/logging.h>

#include "base/macros.h"
#include "base/eintr_wrapper.h"

namespace base {

struct linux_dirent {
  uint64_t	d_ino;
  uint64_t  	d_off;
  unsigned short d_reclen;
  unsigned char d_type;
  char          d_name[0];
};

class DirReader {
 public:
  explicit DirReader(const char* directory_path)
    : fd_(open(directory_path, O_RDONLY | O_DIRECTORY)),
      offset_(0),
      size_(0) {
    memset(buf_, 0, sizeof(buf_));     
  }

  ~DirReader() {
    if (fd_ >= 0) {
      if (IGNORE_EINTR(close(fd_))) {
      }
    }
  }

  bool IsValid() const {
    return fd_ >= 0;
  }

  bool Next() {
    if (size_) {
      linux_dirent* dirent = reinterpret_cast<linux_dirent*>(&buf_[offset_]);
      offset_ += dirent->d_reclen;
    }

    if (offset_ != size_)
      return true;

    const int r = syscall(__NR_getdents64, fd_, buf_, sizeof(buf_));
    if (r == 0) {
      return false;
    }
    if (r == -1) {
      return false;
    }
    size_ = r;
    offset_ = 0;
    return true;
  }

  const char* name() const {
    if (!size_) {
      return nullptr;
    }

    const linux_dirent* dirent =
      reinterpret_cast<const linux_dirent*>(&buf_[offset_]);
    return dirent->d_name;
  }

  int fd() const {
    return fd_;
  }

  static bool IsFallback() {
    return false;
  }

 private:
  const int fd_;
  unsigned char buf_[512];
  size_t offset_;
  size_t size_;

  DISALLOW_COPY_AND_ASSIGN(DirReader);
};

} // namespace base
#endif // SPARROW_FILES_DIR_READER_H_
