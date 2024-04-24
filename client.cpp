#include "structs.h"

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with server IP

    RRCConnectionSetupRequest_t request;

    request.criticalExtensions.present = RRCConnectionSetupRequest__criticalExtensions_PR_c1; // Assuming choice c1
    request.criticalExtensions.choice.c1.ue_Identity.present = UE_Identity_PR_randomValue; // Assuming random UE identity


    uint8_t buffer[BUFFER_SIZE];
    size_t encoded_size;
    asn_enc_rval_t erval = asn_encode_to_buffer(0, ATS_ALIGNED_BASIC_PER, &asn_DEF_RRCConnectionSetupRequest, &request, buffer, BUFFER_SIZE);
    if (erval.encoded == -1) {
        fprintf(stderr, "Cannot encode %s: %s\n", erval.failed_type->name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    encoded_size = erval.encoded;

    sendto(sockfd, buffer, encoded_size, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    printf("RRC Connection Setup Request sent!\n");


    uint8_t buffer[BUFFER_SIZE];
    struct sockaddr_in serverAddr; // Assuming you want to verify server address
    socklen_t serverAddrLen = sizeof(serverAddr);
    ssize_t received_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddr, &serverAddrLen);
    if (received_size < 0) {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }

    asn_dec_rval_t rval;
    RRCConnectionSetupComplete_t *response = 0;
    rval = asn_decode(0, ATS_ALIGNED_BASIC_PER, &asn_DEF_RRCConnectionSetupComplete, (void*)&response, buffer, received_size);
    if (rval.code != RC_OK) {
        fprintf(stderr, "Decode failed: %s\n", rval.code == RC_WMORE ? "Unexpected end of input" : "Input processing error");
        exit(EXIT_FAILURE);
    }


    printf("Received RRC Connection Setup Complete:\n");

    asn_DEF_RRCConnectionSetupComplete.free_struct(&asn_DEF_RRCConnectionSetupComplete, response, 0);

    close(sockfd);

    return 0;
}
