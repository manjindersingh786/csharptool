using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using System.Drawing;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        private bool logEmpty = false;
        public Form1()
        {
            CardTool.establishConnection();
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(DialogResult.OK == openFileDialog1.ShowDialog())
            {
                textBox1.Text = openFileDialog1.FileName;
            }
        }

        private void folderBrowserDialog1_HelpRequest(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }

        private void tabControl_DrawItem(object sender, DrawItemEventArgs e)
        {
            string tabName = tabControl1.TabPages[e.Index].Text;

            StringFormat stringFormat = new StringFormat();

            stringFormat.LineAlignment = StringAlignment.Center;

            //Find if it is selected, this one will be hightlighted...
            if (e.Index == tabControl1.SelectedIndex)
                e.Graphics.FillRectangle(Brushes.Beige, e.Bounds);

            RectangleF rf = tabControl1.GetTabRect(e.Index);

            e.Graphics.DrawString(tabName, this.Font, Brushes.Black, rf, stringFormat);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string wPath = @"out.scr";
            string l;

            /*create a output file*/
            File.Create(wPath).Close();

            //Output file to open ins stream
            StreamWriter wfile = new StreamWriter(wPath);

            //Read all lines
            var lines = File.ReadAllLines(textBox1.Text);

            foreach (var line in lines)
            {
                l = line;
                //check line must not be started with comment
                if (!l.StartsWith(@"//"))
                {
                    //if its not a commented line
                    //append ; in the end of line
                    l = line.Insert(line.Length, ";");

                }

                //Add line in output file
                wfile.WriteLine(l);

            }

            //Close the output stream
            wfile.Close();

            //Delete the input script file
            File.Delete(textBox1.Text);

            //create a new file and copy the content of output script file
            File.Copy(wPath, textBox1.Text);

            //Delete temp file
            File.Delete(wPath);

            label1.Text = @"File Converted";
        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            if(compRadioButton.Checked == true)
            {
                sresLabel.ForeColor = Color.Black;
                kcLabel.ForeColor = Color.Black;
                opText.Enabled = false;
                amfText.Enabled = false;
                sqnText.Enabled = false;
                opLabel.ForeColor = SystemColors.GrayText;
                sqnLabel.ForeColor = SystemColors.GrayText;
                amfLabel.ForeColor = SystemColors.GrayText;
                macLabel.ForeColor = SystemColors.GrayText;
            }
            else
            {
                sresLabel.ForeColor = SystemColors.GrayText;
                kcLabel.ForeColor = SystemColors.GrayText;
            }
        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            if (milenageRadioButton.Checked == true)
            {
                resLabel.ForeColor = Color.Black;
                opcLabel.ForeColor = Color.Black;
                ckLabel.ForeColor = Color.Black;
                ikLabel.ForeColor = Color.Black;
                akLabel.ForeColor = Color.Black;
                autnLabel.ForeColor = Color.Black;
                macLabel.ForeColor = Color.Black;
                opText.Enabled = true;
                amfText.Enabled = true;
                sqnText.Enabled = true;
                opLabel.ForeColor = SystemColors.WindowText;
                sqnLabel.ForeColor = SystemColors.WindowText;
                amfLabel.ForeColor = SystemColors.WindowText;
            }
            else
            {
                resLabel.ForeColor = SystemColors.GrayText;
                opcLabel.ForeColor = SystemColors.GrayText;
                ckLabel.ForeColor = SystemColors.GrayText;
                ikLabel.ForeColor = SystemColors.GrayText;
                akLabel.ForeColor = SystemColors.GrayText;
                autnLabel.ForeColor = SystemColors.GrayText;
            }
        }
        private static byte[] StringToByteArrayFastest(string hex)
        {
            byte[] arr = new byte[hex.Length >> 1];

            for (int i = 0; i < arr.Length; ++i)
            {
                arr[i] = (byte)((GetHexVal(hex[i << 1]) << 4) + (GetHexVal(hex[(i << 1) + 1])));
            }

            return arr;
        }

        private static int GetHexVal(char hex)
        {
            int val = (int)hex;
            //For uppercase A-F letters:
            //return val - (val < 58 ? 48 : 55);
            //For lowercase a-f letters:
            //return val - (val < 58 ? 48 : 87);
            //Or the two combined, but a bit slower:
            return val - (val < 58 ? 48 : (val < 97 ? 55 : 87));
        }

        private void button3_Click(object sender, EventArgs e)
        {
            errorLabel.Text = "";
            if (true == compRadioButton.Checked)
            {
                if(kText.TextLength != 32)
                {
                    errorLabel.Text = "Invalid length of K, must be 16 bytes";
                    return;

                }else if (randText.TextLength != 32)
                {
                    errorLabel.Text = "Invalid length of RAND, must be 16 bytes";
                    return;
                }

                byte[] random = StringToByteArrayFastest(randText.Text);
                byte[] key = StringToByteArrayFastest(kText.Text);
                byte[] sres = new byte[4];
                byte[] kc = new byte[8];

                comp128.algos_comp128v3(key, random, sres, kc);

                sresText.Text = BitConverter.ToString(sres).Replace("-", "");
                kcText.Text = BitConverter.ToString(kc).Replace("-", "");
                resText.Text = "";
                ckText.Text = "";
                ikText.Text = "";
                akText.Text = "";
                opcText.Text = "";
                macText.Text = "";
                autnText.Text = "";
            }
            else // Milenage
            {
                if (opText.TextLength != 32)
                {
                    errorLabel.Text = "Invalid length of OP, must be 16 bytes";
                    return;

                }
                else if (kText.TextLength != 32)
                {
                    errorLabel.Text = "Invalid length of K, must be 16 bytes";
                    return;

                }
                else if (randText.TextLength != 32)
                {
                    errorLabel.Text = "Invalid length of RAND, must be 16 bytes";
                    return;
                }
                else if (sqnText.TextLength != 12)
                {
                    errorLabel.Text = "Invalid length of SQN, must be 6 bytes";
                    return;
                }
                else if (amfText.TextLength != 4)
                {
                    errorLabel.Text = "Invalid length of AMF, must be 4 bytes";
                    return;
                }

                byte[] random = StringToByteArrayFastest(randText.Text);
                byte[] key = StringToByteArrayFastest(kText.Text);
                byte[] op = StringToByteArrayFastest(opText.Text);
                byte[] sqn = StringToByteArrayFastest(sqnText.Text);
                byte[] amf = StringToByteArrayFastest(amfText.Text);
                byte[] ck = new byte[0x10];
                byte[] ik = new byte[0x10];
                byte[] ak = new byte[0x06];
                byte[] res = new byte[0x08];
                byte[] autn = new byte[0x10];

                Milenage.setOpc(op, key);

                byte[] mac = Milenage.f1(key, random, sqn, amf);
                Milenage.f2345(key, random, res, ck, ik, ak);
                resText.Text = BitConverter.ToString(res).Replace("-", "");
                ckText.Text = BitConverter.ToString(ck).Replace("-", "");
                ikText.Text = BitConverter.ToString(ik).Replace("-", "");
                akText.Text = BitConverter.ToString(ak).Replace("-", "");
                opcText.Text = BitConverter.ToString(Milenage.opc).Replace("-", "");
                macText.Text = BitConverter.ToString(mac).Replace("-", "");
                for (byte i = 0; i < 6; i++)
                {
                    autn[i] = (byte)(sqn[i] ^ ak[i]);
                }

                autn[0x06] = amf[0];
                autn[0x07] = amf[1];

                for (byte i = 0; i < 8; i++)
                {
                    autn[i+0x08] = mac[i];
                }
                autnText.Text = BitConverter.ToString(autn).Replace("-", "");

                sresText.Text = "";
                kcText.Text = "";
            }
        }

        private void hexKeyOnly(System.Windows.Forms.KeyPressEventArgs e)
        {
            char c = e.KeyChar;
            if (!(
                    (c >= '0' && c <= '9') 
                    || 
                    (c >= 'A' && c <= 'F') 
                    || 
                    (c >= 'a' && c <= 'f')
                    ||
                    Char.IsControl(c)
                  )
               )
            {
                e.Handled = true;
            }
        }
        private void KText_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }


        private void OpText_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }

        private void kText_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }

        private void RandText_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }

        private void SqnText_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }

        private void AmfText_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            errLabel2.Text = "";

            if (autnText2.TextLength != 32)
            {
                errLabel2.Text = "Invalid length of AUTN, must be 16 bytes";
                return;
            }
            else if (kText2.TextLength != 32)
            {
                errLabel2.Text = "Invalid length of K, must be 16 bytes";
                return;

            }
            else if (randText2.TextLength != 32)
            {
                errLabel2.Text = "Invalid length of RAND, must be 16 bytes";
                return;
            }
            else if (opText2.TextLength != 32)
            {
                errLabel2.Text = "Invalid length of OP, must be 16 bytes";
                return;
            }

            byte[] random = StringToByteArrayFastest(randText2.Text);
            byte[] key = StringToByteArrayFastest(kText2.Text);
            byte[] op = StringToByteArrayFastest(opText2.Text);
            byte[] autn = StringToByteArrayFastest(autnText2.Text);
            byte[] amf = new byte[0x02];
            byte[] ck = new byte[0x10];
            byte[] ik = new byte[0x10];
            byte[] ak = new byte[0x06];
            byte[] res = new byte[0x08];
            byte[] sqn = new byte[0x06];

            Milenage.setOpc(op, key);

            Milenage.f2345(key, random, res, ck, ik, ak);

            for(byte i = 0; i < 0x06; i++)
            {
                sqn[i] = (byte)(autn[i] ^ ak[i]);
            }
            amf[0] = autn[0x06];
            amf[1] = autn[0x07];
            byte[] mac = Milenage.f1(key, random, sqn, amf);

            for(byte i = 0; i < 0x08; i++)
            {
                if (mac[i] != autn[0x08 + i])
                {
                    macResultLabel.ForeColor = Color.Red;
                    macResultLabel.Text = "MAC Verification Failed, calcuated one is " + BitConverter.ToString(mac).Replace("-", "");
                    resText2.Text = "";
                    ckText2.Text = "";
                    ikText2.Text = "";
                    akText2.Text = "";
                    opcText2.Text = "";
                    sqnText2.Text = "";
                    return;
                }
            }

            macResultLabel.ForeColor = Color.Green;
            macResultLabel.Text = "MAC Verification Passed";


            resText2.Text = BitConverter.ToString(res).Replace("-", "");
            ckText2.Text = BitConverter.ToString(ck).Replace("-", "");
            ikText2.Text = BitConverter.ToString(ik).Replace("-", "");
            akText2.Text = BitConverter.ToString(ak).Replace("-", "");
            opcText2.Text = BitConverter.ToString(Milenage.opc).Replace("-", "");
            sqnText2.Text = BitConverter.ToString(sqn).Replace("-", "");
        }

        private void opText_TextChanged(object sender, EventArgs e)
        {
            countOpLabel.Text = (opText.TextLength / 2).ToString() + " b" ;
        }

        private void kText_TextChanged(object sender, EventArgs e)
        {
            countKLabel.Text = (kText.TextLength / 2).ToString() + " b" ;
        }

        private void randText_TextChanged(object sender, EventArgs e)
        {
            countRandLabel.Text = (randText.TextLength / 2).ToString() + " b" ;
        }

        private void sqnText_TextChanged(object sender, EventArgs e)
        {
            countSqnLabel.Text = (sqnText.TextLength / 2).ToString() + " b" ;
        }

        private void amfText_TextChanged(object sender, EventArgs e)
        {
            countAmfLabel.Text = (amfText.TextLength / 2).ToString() + " b" ;
        }

        private void autnText2_TextChanged(object sender, EventArgs e)
        {
            countAutnLabel.Text = (autnText2.TextLength / 2).ToString() + " b";
        }

        private void kText2_TextChanged(object sender, EventArgs e)
        {
            countKLabel2.Text = (kText2.TextLength / 2).ToString() + " b";
        }

        private void randText2_TextChanged(object sender, EventArgs e)
        {
            countRandLabel2.Text = (randText2.TextLength / 2).ToString() + " b";
        }

        private void opText2_TextChanged(object sender, EventArgs e)
        {
            countOpLabel2.Text = (opText2.TextLength / 2).ToString() + " b";
        }
         private void OpText2_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }
        
        private void RandText2_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }

        private void KText2_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }

        private void AutnText2_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }
        
        private void ReadersComboBox_DropDown(object sender, System.EventArgs e)
        {
            //Clean the previous data
            readersComboBox.Items.Clear();

            //Get list of new readers connected right now
            var readers= CardTool.listOfreaders();

            //traverse each reader name one by one
            foreach(string var in readers)
            {
                //Add reader name in list
                readersComboBox.Items.Add(var);
            }
        }

        private void commandTextBox_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            hexKeyOnly(e);
        }
        private void printLineInLog(string str, Color clr)
        {
            if(false == logEmpty)
            {
                LogTextBox.Text = "A";
                int textHeight;
                using (Graphics g = LogTextBox.CreateGraphics())
                {
                    textHeight = TextRenderer.MeasureText(g, LogTextBox.Text, LogTextBox.Font).Height;
                }
                LogTextBox.Text = "";
                for (int i = 0; i < ((LogTextBox.Height / textHeight)-3); i++)
                {
                    LogTextBox.AppendText("\r\n");
                }
                logEmpty = true;
            }


            LogTextBox.SelectionLength = 0;
            LogTextBox.SelectionStart = LogTextBox.TextLength;
            LogTextBox.SelectionColor = clr;

            LogTextBox.ScrollToCaret();
            LogTextBox.AppendText(str);
            LogTextBox.SelectionColor = LogTextBox.ForeColor;
            LogTextBox.AppendText(Environment.NewLine);
        }
        private void printCmdLineInLog(string str)
        {
            LogTextBox.AppendText("<- ");
            printLineInLog(str, Color.DarkOrange);
        }

        private void printRspLineInLog(string str)
        {
            LogTextBox.AppendText("-> ");
            printLineInLog(str, Color.Green);
        }

        private void printErrorLineInLog(string str)
        {
            LogTextBox.AppendText("");
            printLineInLog(str, Color.Green);
        }
        private void resetButton_Click(object sender, EventArgs e)
        {

            try
            {
                //Select card reader
                CardTool.setReader(readersComboBox.Text);
            }catch(Exception ex)
            {
                LogTextBox.Text = ex.Message;
                return;
            }

            //Append command
            //LogTextBox.AppendText("<- ");
            printLineInLog("<- Reset", Color.Blue);
            
            //Reset the card reader
            var atr = CardTool.ResetCard();

            //append the response in log
            printRspLineInLog(atr);

        }

        private void sendCommandButton_Click(object sender, EventArgs e)
        {
            byte[][] commands = new byte[commandTextBox.Lines.Length][];
            byte[] responseBuffer;
            int i = 0;
            foreach(var command in commandTextBox.Lines)
            {
                if(10 > command.Length)
                {
                    printLineInLog("Complete command header missing in command no. " + (i + 1).ToString(), Color.Red);
                    return;
                }

                commands[i] = StringToByteArrayFastest(command);
                
                if ((commands[i].Length > 5) && (commands[i][4] != (commands[i].Length - 5)))
                {
                    printLineInLog("Complete command header missing in command no. " + (i + 1).ToString(), Color.Red);
                    return;
                }
                i++;
            }

            commandTextBox.Text = "";

            for (i = 0; i < commands.Length; i++)
            {
                printCmdLineInLog(BitConverter.ToString(commands[i]).Replace("-","  "));
                try
                {
                    responseBuffer = CardTool.sendCommand(commands[i]);
                }catch(Exception ex)
                {
                    printLineInLog(ex.Message, Color.Red);
                    return;
                }
                

                if(responseBuffer.Length > 2)
                {
                    var remLength = responseBuffer.Length - 2;
                    int startIndex = 0;
                    var rowLength = 16;
                    while (remLength > rowLength)
                    {
                        printRspLineInLog(BitConverter.ToString(responseBuffer, startIndex, rowLength).Replace("-", "\t"));
                        remLength -= rowLength;
                        startIndex += rowLength;
                    }

                    printRspLineInLog(BitConverter.ToString(responseBuffer, startIndex, remLength).Replace("-", "\t"));
                }
                
                printRspLineInLog("SW: "+ BitConverter.ToString(responseBuffer, 0, 0x02).Replace("-", " "));
            }
        }
    }
}
