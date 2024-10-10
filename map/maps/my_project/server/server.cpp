#include <grpcpp/grpcpp.h>
#include "proto/image_service.grpc.pb.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>
#include <vector>
#include "../../../../hsm_client/proto/encryption.grpc.pb.h" 
#include "../../../../hsm_client/proto/encryption.pb.h" 
#include "../../../../hsm_client/include/crypto_api.h"
#include "../../../../hsm_client/include/debug_utils.h"

// הגדר את הפורט של קליינט ההצפנה
const std::string ENCRYPTION_SERVER_ADDRESS = "10.0.0.4:50051";

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;

class ImageServiceImpl final : public image_service::ImageService::Service 
{
public:
    grpc::Status GetImage(grpc::ServerContext* context, const image_service::ImageRequest* request,
                          image_service::ImageResponse* response) override 
    {
        int x = request->x();
        int y = request->y();

        std::stringstream imagePath;
        imagePath << "../tiles/tile_" << x << "_" << y << ".jpg";

        std::ifstream imgFile(imagePath.str(), std::ios::binary);
        if (!imgFile) {
            std::cout<<imagePath.str()<<std::endl;
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "Image not found");
        }

        std::vector<uint8_t> imageData((std::istreambuf_iterator<char>(imgFile)), std::istreambuf_iterator<char>());

        CryptoClient client(grpc::CreateChannel(ENCRYPTION_SERVER_ADDRESS, grpc::InsecureChannelCredentials()));
        CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128, AESChainingMode::CFB, AsymmetricFunction::RSA);
        client.configure(1, config);
        client.configure(2, config);
        client.bootSystem({{1, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}},
                               {2, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}}});
            size_t encryptedLength = client.getEncryptedLenClient(1, imageData.size());
            if (encryptedLength == 0) {
                std::cerr << "Error: Encrypted length is zero or invalid." << std::endl;
                return Status(grpc::INTERNAL, "Invalid encrypted length");
            }

            std::vector<uint8_t> encryptedData(encryptedLength);
            CK_RV encryptResult = client.encrypt(1, 2, (void*)imageData.data(), imageData.size(), encryptedData.data(), encryptedLength);
            if (encryptResult != CKR_OK) {
                std::cerr << "Encryption failed." << std::endl;
                return Status(grpc::INTERNAL, "Encryption failed");
            }

            std::cerr << "Successfully encrypted data. Encrypted length: " << encryptedLength << std::endl;
        std::cout<<imageData.size()<<"omageData"<<std::endl;

    response->set_image_data(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());

        return grpc::Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50052");
    ImageServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}