#include "structs.h"

int main()
{

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr{};
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces

    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        uint8_t buffer[BUFFER_SIZE];
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        ssize_t received_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (received_size < 0) {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        asn_dec_rval_t rval;
        RRCConnectionSetupComplete_t *request = 0;
        rval = asn_decode(0, ATS_ALIGNED_BASIC_PER, &asn_DEF_RRCConnectionSetupRequest, (void*)&request, buffer, received_size);
        if (rval.code != RC_OK) {
            fprintf(stderr, "Decode failed: %s\n", rval.code == RC_WMORE ? "Unexpected end of input" : "Input processing error");
            exit(EXIT_FAILURE);
        }


        printf("Received RRC Connection Setup Request from UE:\n");


        if (request->criticalExtensions.present == RRCConnectionSetupRequest__criticalExtensions_PR_c1) {
            if (request->criticalExtensions.choice.c1.ue_Identity.present == UE_Identity_PR_randomValue) {
                printf("UE Identity (Random Value): ...\n"); // Access and print the random value
            }
            else
            if (true)
            {

            }
        }

        asn_DEF_RRCConnectionSetupComplete.free_struct(&asn_DEF_RRCConnectionSetupRequest, request, 0);
    }


    RRCConnectionSetupComplete_t response;


    response.criticalExtensions.present = RRCConnectionSetupComplete__criticalExtensions_PR_c1;
    response.criticalExtensions.choice.c1.newUE_Identity = 12345; // Assign a new UE identity


    uint8_t buffer[BUFFER_SIZE];
    size_t encoded_size;
    asn_enc_rval_t erval = asn_encode_to_buffer(0, ATS_ALIGNED_BASIC_PER, &asn_DEF_RRCConnectionSetupComplete, &response, buffer, BUFFER_SIZE);
    if (erval.encoded == -1) {
        fprintf(stderr, "Cannot encode %s: %s\n", erval.failed_type->name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    encoded_size = erval.encoded;

    sendto(sockfd, buffer, encoded_size, 0, (struct sockaddr*)&clientAddr, clientAddrLen);

    printf("RRC Connection Setup Complete sent!\n");

    close(sockfd);

    return 0;
}
