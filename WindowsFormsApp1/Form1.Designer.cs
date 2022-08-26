using System.Drawing;
namespace WindowsFormsApp1
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.button2 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.akLabel = new System.Windows.Forms.Label();
            this.ikLabel = new System.Windows.Forms.Label();
            this.ckLabel = new System.Windows.Forms.Label();
            this.resLabel = new System.Windows.Forms.Label();
            this.opcLabel = new System.Windows.Forms.Label();
            this.kcLabel = new System.Windows.Forms.Label();
            this.sresLabel = new System.Windows.Forms.Label();
            this.akText = new System.Windows.Forms.TextBox();
            this.ikText = new System.Windows.Forms.TextBox();
            this.ckText = new System.Windows.Forms.TextBox();
            this.resText = new System.Windows.Forms.TextBox();
            this.opcText = new System.Windows.Forms.TextBox();
            this.kcText = new System.Windows.Forms.TextBox();
            this.sresText = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.button3 = new System.Windows.Forms.Button();
            this.amfLabel = new System.Windows.Forms.Label();
            this.amfText = new System.Windows.Forms.TextBox();
            this.sqnLabel = new System.Windows.Forms.Label();
            this.sqnText = new System.Windows.Forms.TextBox();
            this.randLabel = new System.Windows.Forms.Label();
            this.randText = new System.Windows.Forms.TextBox();
            this.kLabel = new System.Windows.Forms.Label();
            this.kText = new System.Windows.Forms.TextBox();
            this.opLabel = new System.Windows.Forms.Label();
            this.opText = new System.Windows.Forms.TextBox();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.label2 = new System.Windows.Forms.Label();
            this.tabControl1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(36, 84);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(488, 22);
            this.textBox1.TabIndex = 0;
            this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(205, 122);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(100, 29);
            this.button1.TabIndex = 1;
            this.button1.Text = "Browse..";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog1_FileOk);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(205, 171);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(100, 36);
            this.button2.TabIndex = 2;
            this.button2.Text = "Convert";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(252, 224);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(0, 16);
            this.label1.TabIndex = 3;
            // 
            // tabControl1
            // 
            this.tabControl1.Alignment = System.Windows.Forms.TabAlignment.Left;
            this.tabControl1.AllowDrop = true;
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.DrawMode = System.Windows.Forms.TabDrawMode.OwnerDrawFixed;
            this.tabControl1.ItemSize = new System.Drawing.Size(30, 100);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Multiline = true;
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.Padding = new System.Drawing.Point(0, 0);
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(856, 446);
            this.tabControl1.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
            this.tabControl1.TabIndex = 0;
            this.tabControl1.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.tabControl_DrawItem);
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage2.Controls.Add(this.groupBox2);
            this.tabPage2.Controls.Add(this.groupBox1);
            this.tabPage2.Controls.Add(this.button3);
            this.tabPage2.Controls.Add(this.amfLabel);
            this.tabPage2.Controls.Add(this.amfText);
            this.tabPage2.Controls.Add(this.sqnLabel);
            this.tabPage2.Controls.Add(this.sqnText);
            this.tabPage2.Controls.Add(this.randLabel);
            this.tabPage2.Controls.Add(this.randText);
            this.tabPage2.Controls.Add(this.kLabel);
            this.tabPage2.Controls.Add(this.kText);
            this.tabPage2.Controls.Add(this.opLabel);
            this.tabPage2.Controls.Add(this.opText);
            this.tabPage2.Location = new System.Drawing.Point(104, 4);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(748, 438);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Authentication";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.akLabel);
            this.groupBox2.Controls.Add(this.ikLabel);
            this.groupBox2.Controls.Add(this.ckLabel);
            this.groupBox2.Controls.Add(this.resLabel);
            this.groupBox2.Controls.Add(this.opcLabel);
            this.groupBox2.Controls.Add(this.kcLabel);
            this.groupBox2.Controls.Add(this.sresLabel);
            this.groupBox2.Controls.Add(this.akText);
            this.groupBox2.Controls.Add(this.ikText);
            this.groupBox2.Controls.Add(this.ckText);
            this.groupBox2.Controls.Add(this.resText);
            this.groupBox2.Controls.Add(this.opcText);
            this.groupBox2.Controls.Add(this.kcText);
            this.groupBox2.Controls.Add(this.sresText);
            this.groupBox2.Location = new System.Drawing.Point(32, 191);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(660, 232);
            this.groupBox2.TabIndex = 14;
            this.groupBox2.TabStop = false;
            // 
            // akLabel
            // 
            this.akLabel.AutoSize = true;
            this.akLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.akLabel.Location = new System.Drawing.Point(20, 201);
            this.akLabel.Name = "akLabel";
            this.akLabel.Size = new System.Drawing.Size(24, 16);
            this.akLabel.TabIndex = 13;
            this.akLabel.Text = "AK";
            // 
            // ikLabel
            // 
            this.ikLabel.AutoSize = true;
            this.ikLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.ikLabel.Location = new System.Drawing.Point(20, 174);
            this.ikLabel.Name = "ikLabel";
            this.ikLabel.Size = new System.Drawing.Size(18, 16);
            this.ikLabel.TabIndex = 12;
            this.ikLabel.Text = "IK";
            // 
            // ckLabel
            // 
            this.ckLabel.AutoSize = true;
            this.ckLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.ckLabel.Location = new System.Drawing.Point(20, 147);
            this.ckLabel.Name = "ckLabel";
            this.ckLabel.Size = new System.Drawing.Size(24, 16);
            this.ckLabel.TabIndex = 11;
            this.ckLabel.Text = "CK";
            // 
            // resLabel
            // 
            this.resLabel.AutoSize = true;
            this.resLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.resLabel.Location = new System.Drawing.Point(20, 121);
            this.resLabel.Name = "resLabel";
            this.resLabel.Size = new System.Drawing.Size(35, 16);
            this.resLabel.TabIndex = 10;
            this.resLabel.Text = "RES";
            // 
            // opcLabel
            // 
            this.opcLabel.AutoSize = true;
            this.opcLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.opcLabel.Location = new System.Drawing.Point(20, 95);
            this.opcLabel.Name = "opcLabel";
            this.opcLabel.Size = new System.Drawing.Size(35, 16);
            this.opcLabel.TabIndex = 9;
            this.opcLabel.Text = "OPC";
            // 
            // kcLabel
            // 
            this.kcLabel.AutoSize = true;
            this.kcLabel.ForeColor = System.Drawing.Color.Black;
            this.kcLabel.Location = new System.Drawing.Point(20, 56);
            this.kcLabel.Name = "kcLabel";
            this.kcLabel.Size = new System.Drawing.Size(22, 16);
            this.kcLabel.TabIndex = 8;
            this.kcLabel.Text = "Kc";
            // 
            // sresLabel
            // 
            this.sresLabel.AutoSize = true;
            this.sresLabel.ForeColor = System.Drawing.Color.Black;
            this.sresLabel.Location = new System.Drawing.Point(20, 27);
            this.sresLabel.Name = "sresLabel";
            this.sresLabel.Size = new System.Drawing.Size(44, 16);
            this.sresLabel.TabIndex = 7;
            this.sresLabel.Text = "SRES";
            // 
            // akText
            // 
            this.akText.Enabled = false;
            this.akText.Location = new System.Drawing.Point(107, 195);
            this.akText.Name = "akText";
            this.akText.Size = new System.Drawing.Size(281, 22);
            this.akText.TabIndex = 6;
            // 
            // ikText
            // 
            this.ikText.Enabled = false;
            this.ikText.Location = new System.Drawing.Point(107, 168);
            this.ikText.Name = "ikText";
            this.ikText.Size = new System.Drawing.Size(281, 22);
            this.ikText.TabIndex = 5;
            // 
            // ckText
            // 
            this.ckText.Enabled = false;
            this.ckText.Location = new System.Drawing.Point(107, 141);
            this.ckText.Name = "ckText";
            this.ckText.Size = new System.Drawing.Size(281, 22);
            this.ckText.TabIndex = 4;
            // 
            // resText
            // 
            this.resText.Enabled = false;
            this.resText.Location = new System.Drawing.Point(107, 115);
            this.resText.Name = "resText";
            this.resText.Size = new System.Drawing.Size(281, 22);
            this.resText.TabIndex = 3;
            // 
            // opcText
            // 
            this.opcText.Enabled = false;
            this.opcText.Location = new System.Drawing.Point(107, 89);
            this.opcText.Name = "opcText";
            this.opcText.Size = new System.Drawing.Size(281, 22);
            this.opcText.TabIndex = 2;
            // 
            // kcText
            // 
            this.kcText.Enabled = false;
            this.kcText.Location = new System.Drawing.Point(106, 50);
            this.kcText.Name = "kcText";
            this.kcText.Size = new System.Drawing.Size(281, 22);
            this.kcText.TabIndex = 1;
            // 
            // sresText
            // 
            this.sresText.Enabled = false;
            this.sresText.Location = new System.Drawing.Point(105, 24);
            this.sresText.Name = "sresText";
            this.sresText.Size = new System.Drawing.Size(281, 22);
            this.sresText.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioButton1);
            this.groupBox1.Controls.Add(this.radioButton2);
            this.groupBox1.Location = new System.Drawing.Point(543, 9);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(149, 83);
            this.groupBox1.TabIndex = 13;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Algorithms";
            // 
            // radioButton1
            // 
            this.radioButton1.AutoSize = true;
            this.radioButton1.Checked = true;
            this.radioButton1.Location = new System.Drawing.Point(6, 21);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.Size = new System.Drawing.Size(109, 20);
            this.radioButton1.TabIndex = 11;
            this.radioButton1.TabStop = true;
            this.radioButton1.Text = "COMP-128 v3";
            this.radioButton1.UseVisualStyleBackColor = true;
            this.radioButton1.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
            // 
            // radioButton2
            // 
            this.radioButton2.AutoSize = true;
            this.radioButton2.Location = new System.Drawing.Point(6, 47);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(84, 20);
            this.radioButton2.TabIndex = 12;
            this.radioButton2.Text = "Milenage";
            this.radioButton2.UseVisualStyleBackColor = true;
            this.radioButton2.CheckedChanged += new System.EventHandler(this.radioButton2_CheckedChanged);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(303, 162);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(115, 23);
            this.button3.TabIndex = 1;
            this.button3.Text = "Calculate";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // amfLabel
            // 
            this.amfLabel.AutoSize = true;
            this.amfLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.amfLabel.Location = new System.Drawing.Point(52, 136);
            this.amfLabel.Name = "amfLabel";
            this.amfLabel.Size = new System.Drawing.Size(35, 16);
            this.amfLabel.TabIndex = 10;
            this.amfLabel.Text = "AMF";
            // 
            // amfText
            // 
            this.amfText.Enabled = false;
            this.amfText.Location = new System.Drawing.Point(139, 133);
            this.amfText.MaxLength = 4;
            this.amfText.Name = "amfText";
            this.amfText.Size = new System.Drawing.Size(87, 22);
            this.amfText.TabIndex = 9;
            this.amfText.KeyPress += AmfText_KeyPress;
            // 
            // sqnLabel
            // 
            this.sqnLabel.AutoSize = true;
            this.sqnLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.sqnLabel.Location = new System.Drawing.Point(52, 105);
            this.sqnLabel.Name = "sqnLabel";
            this.sqnLabel.Size = new System.Drawing.Size(36, 16);
            this.sqnLabel.TabIndex = 8;
            this.sqnLabel.Text = "SQN";
            // 
            // sqnText
            // 
            this.sqnText.Enabled = false;
            this.sqnText.Location = new System.Drawing.Point(139, 102);
            this.sqnText.MaxLength = 12;
            this.sqnText.Name = "sqnText";
            this.sqnText.Size = new System.Drawing.Size(207, 22);
            this.sqnText.TabIndex = 7;
            this.sqnText.KeyPress += SqnText_KeyPress;
            // 
            // randLabel
            // 
            this.randLabel.AutoSize = true;
            this.randLabel.Location = new System.Drawing.Point(52, 73);
            this.randLabel.Name = "randLabel";
            this.randLabel.Size = new System.Drawing.Size(46, 16);
            this.randLabel.TabIndex = 6;
            this.randLabel.Text = "RAND";
            // 
            // randText
            // 
            this.randText.Location = new System.Drawing.Point(139, 70);
            this.randText.MaxLength = 32;
            this.randText.Name = "randText";
            this.randText.Size = new System.Drawing.Size(353, 22);
            this.randText.TabIndex = 5;
            this.randText.KeyPress += RandText_KeyPress;
            // 
            // kLabel
            // 
            this.kLabel.AutoSize = true;
            this.kLabel.Location = new System.Drawing.Point(52, 44);
            this.kLabel.Name = "kLabel";
            this.kLabel.Size = new System.Drawing.Size(15, 16);
            this.kLabel.TabIndex = 4;
            this.kLabel.Text = "K";
            // 
            // kText
            // 
            this.kText.Location = new System.Drawing.Point(138, 40);
            this.kText.MaxLength = 32;
            this.kText.Name = "kText";
            this.kText.Size = new System.Drawing.Size(353, 22);
            this.kText.TabIndex = 3;
            this.kText.KeyPress += kText_KeyPress;
            // 
            // opLabel
            // 
            this.opLabel.AutoSize = true;
            this.opLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.opLabel.Location = new System.Drawing.Point(51, 11);
            this.opLabel.Name = "opLabel";
            this.opLabel.Size = new System.Drawing.Size(26, 16);
            this.opLabel.TabIndex = 2;
            this.opLabel.Text = "OP";
            // 
            // opText
            // 
            this.opText.BackColor = System.Drawing.SystemColors.Window;
            this.opText.Enabled = false;
            this.opText.Location = new System.Drawing.Point(138, 8);
            this.opText.MaxLength = 32;
            this.opText.Name = "opText";
            this.opText.Size = new System.Drawing.Size(353, 22);
            this.opText.TabIndex = 1;
            this.opText.KeyPress += OpText_KeyPress;
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage1.Controls.Add(this.label2);
            this.tabPage1.Controls.Add(this.textBox1);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Controls.Add(this.button2);
            this.tabPage1.Controls.Add(this.button1);
            this.tabPage1.Location = new System.Drawing.Point(104, 4);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.tabPage1.Size = new System.Drawing.Size(748, 438);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Convert To Scr";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(33, 55);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(162, 16);
            this.label2.TabIndex = 4;
            this.label2.Text = "Enter path of file to convert";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(880, 481);
            this.Controls.Add(this.tabControl1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "ConvertToScr";
            this.tabControl1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox opText;
        private System.Windows.Forms.Label amfLabel;
        private System.Windows.Forms.TextBox amfText;
        private System.Windows.Forms.Label sqnLabel;
        private System.Windows.Forms.TextBox sqnText;
        private System.Windows.Forms.Label randLabel;
        private System.Windows.Forms.TextBox randText;
        private System.Windows.Forms.Label kLabel;
        private System.Windows.Forms.TextBox kText;
        private System.Windows.Forms.Label opLabel;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.RadioButton radioButton1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox sresText;
        private System.Windows.Forms.TextBox akText;
        private System.Windows.Forms.TextBox ikText;
        private System.Windows.Forms.TextBox ckText;
        private System.Windows.Forms.TextBox resText;
        private System.Windows.Forms.TextBox opcText;
        private System.Windows.Forms.TextBox kcText;
        private System.Windows.Forms.Label sresLabel;
        private System.Windows.Forms.Label akLabel;
        private System.Windows.Forms.Label ikLabel;
        private System.Windows.Forms.Label ckLabel;
        private System.Windows.Forms.Label resLabel;
        private System.Windows.Forms.Label opcLabel;
        private System.Windows.Forms.Label kcLabel;
    }
}

