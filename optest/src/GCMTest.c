/*
Copyright © 2012, Silent Circle
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Any redistribution, use, or modification is done solely for personal 
      benefit and not for any commercial purpose or for monetary gain
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name Silent Circle nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 
#include <stdio.h>
#include <tomcrypt.h>
#include "optest.h"
#include "cryptowrappers.h"
 
 
static char *Msgs[] = { 
    "That Vizzini, he can *fuss*." , 
    "Fuss, fuss... I think he like to scream at *us*.",
    "Probably he means no *harm*. ",
    "He's really very short on *charm*." ,
    "You have a great gift for rhyme." ,
    "Yes, yes, some of the time.",
    "Enough of that.",
    "Fezzik, are there rocks ahead? ",
    "If there are, we all be dead. ",
    "No more rhymes now, I mean it. ",
    "Anybody want a peanut?",
    "short",
    "no",
    "",
    NULL
};

SCLError TestGCM()
{
    SCLError err = kSCLError_NoErr;
    int status = CRYPT_OK;
     uint8_t key[32];

    uint8_t* CT = NULL;
    size_t  CTLen = 0;
    uint8_t* PT = NULL;
    size_t  PTLen = 0;
    unsigned char  T[32];
    unsigned long tagLen = 0;
 
    int i;
    
    register_cipher (&aes_desc);
  
    printf("\nTesting GCM encoding \n\n");

       for(i = 0; Msgs[i] != NULL; i++)
    {
       
        
        unsigned long msgLen;
        uint8_t seqNum[6];
    
        sprng_read(key,sizeof(key),NULL);

        msgLen = strlen(Msgs[i]);
 //       sprintf(seqNum, "%05d ", i);
        
 //       printf("\n%s %3d |%s|\n\n", seqNum, (int)msgLen, Msgs[i]);  
       
          err = GCM_Encrypt( key,    sizeof(key), 
                            seqNum, sizeof(seqNum), 
                            Msgs[i], msgLen, 
                            &CT,    &CTLen, 
                            T,      &tagLen); CKERR;
        
 // Error test        
//         seqNum[0]= seqNum[0]+1;
//        T[0]= T[0]+1;
        
        err = GCM_Decrypt( key,    sizeof(key), 
                            seqNum, sizeof(seqNum), 
                            CT,     CTLen, 
                            T,      tagLen, 
                            &PT,    &PTLen); CKERR;
        

 //       dumpHex(CT, CTLen, 0);
 //       printf("Tag %d: ", (int)tagLen); dumpHex8(T); dumpHex8(T+8); 
 //       printf("\n");
 
        if(msgLen != PTLen)
        {
            printf("ERROR  GCM Decrypt: Expecting %d bytes, got %d\n", (int)msgLen, (int)PTLen );
            RETERR(kSCLError_SelfTestFailed);
        }
        
        if( compareResults( Msgs[i],  PT, msgLen , kResultFormat_Byte, "GCM Decrypt") != CRYPT_OK)
        {
            RETERR(kSCLError_SelfTestFailed);
        }
//        printf(" %5d |%.*s|\n\n",  (int)PTLen,(int)PTLen, PT );  
        
        
        if(CT)
        {
            memset(CT, CTLen, 0);
            XFREE(CT); CT = NULL;
        }
        
        if(PT)
        {
            memset(PT,PTLen, 0);
            XFREE(PT); PT = NULL;
        }

    }
    
done:
    if(status != CRYPT_OK)
        err = sCrypt2SCLError(status);
    
    if(CT)
    {
        memset(CT, CTLen, 0);
        XFREE(CT); CT = NULL;
    }
    
    if(PT)
    {
        memset(PT,PTLen, 0);
        XFREE(PT); PT = NULL;
    }
     
    return err;
    
    
}

 
