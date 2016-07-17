#include <ace/common/Test.h>
#include <ace/model/Helper.h>
#include <cstring>
#include <string>
#include <02_Ok.ac.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<sys/unistd.h>

using namespace ace;

int main(int argc, char * argv[]) {
  int status = 0;
  // Compute the socket path
  std::string cwd(get_current_dir_name());
  std::string pth = cwd + "/s";
  unlink(pth.c_str());
  // Create the socket
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  T(sock >= 0);
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, pth.c_str());
  if (bind(sock, (struct sockaddr *)(& addr), sizeof(addr)) == 0) {
    // Load the configuration
    auto cfg = model::Helper::parseFile<_02_Ok>("02_Ok.lua", false, argc, argv);
    if (cfg != nullptr) {
    } else {
      status = __LINE__;
    }
    // Close the socket
    close(sock);
    unlink(pth.c_str());
  }
  return status;
}
