file(REMOVE_RECURSE
  "CMakeFiles/proto_gen"
  "generated/encryption.grpc.pb.cc"
  "generated/encryption.grpc.pb.h"
  "generated/encryption.pb.cc"
  "generated/encryption.pb.h"
  "generated/image_service.grpc.pb.cc"
  "generated/image_service.grpc.pb.h"
  "generated/image_service.pb.cc"
  "generated/image_service.pb.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/proto_gen.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
