#include <grpcpp/grpcpp.h>
#include "proto/image_service.grpc.pb.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <string>
#include <vector>
#include "../../../../hsm_client/include/crypto_api.h"
#include "../../../../hsm_client/include/debug_utils.h"

void RunClient(int x, int y) {
    std::string address("localhost:50052");
    auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    auto stub = image_service::ImageService::NewStub(channel);

    image_service::ImageRequest request;
    request.set_x(x);
    request.set_y(y);

    image_service::ImageResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub->GetImage(&context, request, &response);

    CryptoClient client(grpc::CreateChannel("10.0.0.4:50051", grpc::InsecureChannelCredentials()));
    std::vector<uint8_t> decryptedData;
    printBufferHexa((unsigned char *)response.image_data().data(), response.image_data().size(), "after-encryption on server");
    size_t decryptedLength = client.getDecryptedLenClient(1, response.image_data().size());
    if (decryptedLength == 0) { 
        std::cerr << "Invalid decrypted length." << std::endl;
        return;
    }
    std::cout<<"len"<<response.image_data().size()<<std::endl;
    uint8_t tempDecryptedData[decryptedLength];
    CK_RV decryptResult = client.decrypt(1, 2, (void*)response.image_data().data(), response.image_data().size(), tempDecryptedData, decryptedLength);
    if (decryptResult != CKR_OK) {
        std::cerr << "Decryption failed for chunk of size: " << response.image_data().size() << std::endl;
        return;
    }

    if (decryptResult==CKR_OK) {
        std::ofstream imgFile("received_image2.jpg", std::ios::binary);
        imgFile.write(reinterpret_cast<const char *>(tempDecryptedData), decryptedLength);
        imgFile.close();
        std::cout << "Image received and saved as 'received_image.jpg'" << std::endl;
    } else {
        std::cerr << "RPC failed: " << status.error_code() << ": " << status.error_message() << std::endl;
    }
}

int main(int argc, char** argv) {
    int x = -122;
    int y =  37;
    RunClient(x, y);
    return 0;
}