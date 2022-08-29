using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using System.Drawing;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
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
                byte[] iv = new byte[0x10];
                
                Milenage.setOpc(op, key);

                byte[] mac = Milenage.f1(key, random, sqn, amf);
                resText.Text = BitConverter.ToString(mac).Replace("-", "");
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


    }
}
