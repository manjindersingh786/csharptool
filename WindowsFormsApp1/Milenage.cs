using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography;
using System.IO;

namespace WindowsFormsApp1
{
    internal class Milenage
    {
        static public byte[] opc;
        static byte[] Encrypt(byte[] data, byte[] Key)
        {
            byte[] encrypted;
            byte[] IV = new byte[Key.Length];
            // Create a new AesManaged.    
            using (Aes aes = Aes.Create())
            {
                aes.Padding = PaddingMode.Zeros;
                // Create encryptor    
                ICryptoTransform encryptor = aes.CreateEncryptor(Key, IV);
                // Create MemoryStream    
                using (MemoryStream ms = new MemoryStream())
                {
                    // Create crypto stream using the CryptoStream class. This class is the key to encryption    
                    // and encrypts and decrypts data from any given stream. In this case, we will pass a memory stream    
                    // to encrypt    
                    using (CryptoStream cs = new CryptoStream(ms, encryptor, CryptoStreamMode.Write))
                    {
                        cs.Write(data, 0x00, data.Length);
                        cs.FlushFinalBlock();
                        encrypted = ms.ToArray();
                    }
                }
            }
            // Return encrypted data    
            return encrypted;
        }

        static public void setOpc(byte[] OP, byte[] key)
        {
            /*Encryption of OP using Key*/
            opc = Encrypt(OP, key);
            for (byte i = 0; i < 16; i++)
            {
                opc[i] ^= OP[i];
            }

        }
        /*-------------------------------------------------------------------
         * Algorithm f1
         *-------------------------------------------------------------------
         *
         * Computes network authentication code MAC-A from key K, random
         * challenge RAND, sequence number SQN and authentication management
         * field AMF.
         *
         *-----------------------------------------------------------------*/
        static public byte[] f1(byte[] k, byte[] rand, byte[] sqn, byte[] amf)
        {
            byte[] mac_a = new byte[0x08];
            byte[] outp = new byte[0x10];
            byte[] out1 = new byte[0x10];
            byte[] input = new byte[k.Length];
            byte[] input1 = new byte[k.Length];

            for (int i = 0; i < k.Length; i++)
            {
                input[i] = (byte) (rand[i] ^ opc[i]);
            }
            outp = Encrypt(input, k);

            for (int i = 0; i < 6; i++)
            {
                input1[i] = sqn[i];
                input1[i + 8] = sqn[i];
            }
            for (int i = 0; i < 2; i++)
            {
                input1[i + 6] = amf[i];
                input1[i + 14] = amf[i];
            }

            /* XOR opc and in1, rotate by r1=64, and XOR *
            * on the constant c1 (which is all zeroes) */
            for (int i = 0; i < 16; i++)
                input[(i + 8) % 16] = (byte)(input1[i] ^ opc[i]);
            /* XOR on the value temp computed before */
            for (int i = 0; i < 16; i++)
                input[i] ^= outp[i];

            out1 = Encrypt(input, k);

            for (int i = 0; i < 16; i++)
                out1[i] ^= opc[i];
            for (int i = 0; i < 8; i++)
                mac_a[i] = out1[i];

            return mac_a;
        }


        /*-------------------------------------------------------------------
         * Algorithms f2-f5
         *-------------------------------------------------------------------
         *
         * Takes key K and random challenge RAND, and returns response RES,
         * confidentiality key CK, integrity key IK and anonymity key AK.
         *
         *-----------------------------------------------------------------*/
        public static void f2345(byte[] k, byte[] rand, byte[] res, byte[] ck, byte[] ik, byte[] ak)
        {
            byte[] temp = new byte[16];
            byte[] outp = new byte[16];
            byte[] rijndaelInput= new byte[16];
            byte i;
            for (i = 0; i < 16; i++)
                rijndaelInput[i] = (byte)(rand[i] ^ opc[i]);
            
            temp = Encrypt(rijndaelInput, k);
            /* To obtain output block OUT2: XOR OPc and TEMP, *
            * rotate by r2=0, and XOR on the constant c2 (which *
            * is all zeroes except that the last bit is 1). */
            for (i = 0; i < 16; i++)
            {
                rijndaelInput[i] = (byte)(temp[i] ^ opc[i]);
            }
            rijndaelInput[15] ^= 1;
            outp  = Encrypt(rijndaelInput, k);
            for (i = 0; i < 16; i++)
            { 
		        outp[i] ^= opc[i];
            }
            for (i = 0; i < 8; i++)
            {
                res[i] = outp[i +8];
            }
            for (i = 0; i < 6; i++)
            {
                ak[i] = outp[i] ;
            }
            /* To obtain output block OUT3: XOR OPc and TEMP, *
            * rotate by r3=32, and XOR on the constant c3 (which *
            * is all zeroes except that the next to last bit is 1). */
            for (i = 0; i < 16; i++)
            {
                rijndaelInput[(i + 12) % 16] = (byte)(temp[i] ^ opc[i]);
            }
            rijndaelInput[15] ^= 2;
            outp = Encrypt(rijndaelInput, k);
            for (i = 0; i < 16; i++)
            {
                outp[i] ^= opc[i];
            }
            for (i = 0; i < 16; i++)
            {
                ck[i] = outp[i] ;
            }
            /* To obtain output block OUT4: XOR OPc and TEMP, *
            * rotate by r4=64, and XOR on the constant c4 (which *
            * is all zeroes except that the 2nd from last bit is 1). */
            for (i = 0; i < 16; i++)
            {
                rijndaelInput[(i + 8) % 16] = (byte)(temp[i] ^ opc[i]);
            }
            rijndaelInput[15] ^= 4;
            outp = Encrypt(rijndaelInput, k);
            for (i = 0; i < 16; i++)
            {
                outp[i] ^= opc[i];
            }
            for (i = 0; i < 16; i++)
            {
                ik[i] = outp[i];
            }
            return;
        }

        /*-------------------------------------------------------------------
        * Algorithm f1*
        *-------------------------------------------------------------------
        *
        * Computes resynch authentication code MAC-S from key K, random
        * challenge RAND, sequence number SQN and authentication management
        * field AMF.
        *
        *-----------------------------------------------------------------*/
        void f1star(byte[] k, byte[] rand, byte[] sqn, byte[] amf, byte[] mac_s)
        {

        }

        /*-------------------------------------------------------------------
         * Algorithm f5*
         *-------------------------------------------------------------------
         *
         * Takes key K and random challenge RAND, and returns resynch
         * anonymity key AK.
         *
         *-----------------------------------------------------------------*/
        void f5star(byte[] k, byte[] rand, byte[] ak)
        {

        }
    }
}
