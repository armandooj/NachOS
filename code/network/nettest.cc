// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads 
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"
#include <string.h>

// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our 
//	    original message

void showExample(int farAddr) {
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    const char *data = "Hello there!";
    const char *ack = "Got it!";
    char buffer[MaxMailSize];

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = farAddr;		
    outMailHdr.to = 0;
    outMailHdr.from = 1;
    outMailHdr.length = strlen(data) + 1;

    // Send the first message
    postOffice->Send(outPktHdr, outMailHdr, data); 

    // Wait for the first message from the other machine
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);  // Stuck here

    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Send acknowledgement to the other machine (using "reply to" mailbox
    // in the message that just arrived
    outPktHdr.to = inPktHdr.from;
    outMailHdr.to = inMailHdr.from;
    outMailHdr.length = strlen(ack) + 1;
    postOffice->Send(outPktHdr, outMailHdr, ack); 

    // Wait for the ack from the other machine to the first message we sent.
    postOffice->Receive(1, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    printf("Finish here\n");

    // Then we're done!
    interrupt->Halt();
}

void send10Messages(int farAddr){

    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    char data [30];
    char ack [30];
    char buffer[MaxMailSize];

    for (int i = 0; i < 10; i++) {
    
        // construct packet, mail header for original message
        // To: destination machine, mailbox 0
        // From: our machine, reply to: mailbox 1
        sprintf(data, "Message %d", i);
        outPktHdr.to = farAddr;		
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(data) + 1;

        // Send the first message
        postOffice->Send(outPktHdr, outMailHdr, data); 

        // Wait for the first message from the other machine
        postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
        printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
        fflush(stdout);

        // Send acknowledgement to the other machine (using "reply to" mailbox
        // in the message that just arrived
        sprintf(ack, "Got Message %d", i);
        outPktHdr.to = inPktHdr.from;
        outMailHdr.to = inMailHdr.from;
        outMailHdr.length = strlen(ack) + 1;
        postOffice->Send(outPktHdr, outMailHdr, ack); 

        // Wait for the ack from the other machine to the first message we sent.
        postOffice->Receive(1, &inPktHdr, &inMailHdr, buffer);
        printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
        fflush(stdout);
    }
    // Then we're done!
    interrupt->Halt();  
}

void ring3Machines(int farAddr) {
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    char data [40];
    char buffer[MaxMailSize];

    //Making test message
    if (farAddr == 1) {
        sprintf(data, "Message from machine 0");
    } 
    
    if (farAddr == 1) {
        outPktHdr.to = farAddr;
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(data) + 1;

        // Send the first message
        postOffice->Send(outPktHdr, outMailHdr, data); 
    }
    
    // Wait for the first message from the other machine
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);
    
    if (farAddr != 1) {
        strcpy(data, buffer);
        outPktHdr.to = farAddr;
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(data) + 1;

        // Send the first message
        postOffice->Send(outPktHdr, outMailHdr, data); 
    }
    // Then we're done!
    interrupt->Halt();
}


#ifdef CHANGED
void 
sendTest(int farAddr) {

    PacketHeader outPktHdr;
    MailHeader outMailHdr;
    const char *data = "Hello there this is a very big message and I want to see what happens. I think the limit at the moment is 40 so this should be enough.";

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = farAddr;
    outPktHdr.from = 0;     
    outMailHdr.to = 0;
    outMailHdr.from = 1;
    outMailHdr.length = strlen(data) + 1;

    // Send the first message
    postOffice->ReliableSend(outPktHdr, outMailHdr, data); 

    printf("Finish here\n");

    // Then we're done!
    interrupt->Halt();
}

void
waitTest(int farAddr) {
    PacketHeader inPktHdr;
    MailHeader inMailHdr;
    //const char *ack = "Got it!";
    char buffer[MaxMailSize] = {0};
    char bigBuffer[200] = {0};
    // Wait for the first message from the other machine
    postOffice->ReliableReceive(0, &inPktHdr, &inMailHdr, buffer, bigBuffer); 
    // Delay(5);
    
    interrupt->Halt();
}

void
MailSend(int farAddr)                                                                                                                                                                                                                                                                                                       
{
    sendTest(farAddr);
}

void                                                                                                                                                                                                                                                                                                                                                
MailWait(int farAddr)
{
    waitTest(farAddr);
}

#endif

void
MailTest(int farAddr)                                                                                                                                                                                                                                                                                                       
{
    showExample(farAddr);
}


