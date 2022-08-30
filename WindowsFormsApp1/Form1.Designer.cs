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
            this.errorLabel = new System.Windows.Forms.Label();
            this.resultGroupBox = new System.Windows.Forms.GroupBox();
            this.macLabel = new System.Windows.Forms.Label();
            this.macText = new System.Windows.Forms.TextBox();
            this.autnLabel = new System.Windows.Forms.Label();
            this.autnText = new System.Windows.Forms.TextBox();
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
            this.algoGroupBox = new System.Windows.Forms.GroupBox();
            this.compRadioButton = new System.Windows.Forms.RadioButton();
            this.milenageRadioButton = new System.Windows.Forms.RadioButton();
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
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.label3 = new System.Windows.Forms.Label();
            this.opText2 = new System.Windows.Forms.TextBox();
            this.macResultLabel = new System.Windows.Forms.Label();
            this.errLabel2 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label4 = new System.Windows.Forms.Label();
            this.sqnText2 = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.akText2 = new System.Windows.Forms.TextBox();
            this.ikText2 = new System.Windows.Forms.TextBox();
            this.ckText2 = new System.Windows.Forms.TextBox();
            this.resText2 = new System.Windows.Forms.TextBox();
            this.opcText2 = new System.Windows.Forms.TextBox();
            this.button4 = new System.Windows.Forms.Button();
            this.label14 = new System.Windows.Forms.Label();
            this.randText2 = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.kText2 = new System.Windows.Forms.TextBox();
            this.autnLabel2 = new System.Windows.Forms.Label();
            this.autnText2 = new System.Windows.Forms.TextBox();
            this.tabControl1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.resultGroupBox.SuspendLayout();
            this.algoGroupBox.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.groupBox1.SuspendLayout();
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
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.DrawMode = System.Windows.Forms.TabDrawMode.OwnerDrawFixed;
            this.tabControl1.ItemSize = new System.Drawing.Size(30, 100);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Multiline = true;
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.Padding = new System.Drawing.Point(0, 0);
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(845, 510);
            this.tabControl1.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
            this.tabControl1.TabIndex = 0;
            this.tabControl1.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.tabControl_DrawItem);
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage2.Controls.Add(this.errorLabel);
            this.tabPage2.Controls.Add(this.resultGroupBox);
            this.tabPage2.Controls.Add(this.algoGroupBox);
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
            this.tabPage2.Size = new System.Drawing.Size(737, 502);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Authentication";
            // 
            // errorLabel
            // 
            this.errorLabel.AutoSize = true;
            this.errorLabel.Location = new System.Drawing.Point(467, 165);
            this.errorLabel.Name = "errorLabel";
            this.errorLabel.Size = new System.Drawing.Size(0, 16);
            this.errorLabel.TabIndex = 15;
            // 
            // resultGroupBox
            // 
            this.resultGroupBox.Controls.Add(this.macLabel);
            this.resultGroupBox.Controls.Add(this.macText);
            this.resultGroupBox.Controls.Add(this.autnLabel);
            this.resultGroupBox.Controls.Add(this.autnText);
            this.resultGroupBox.Controls.Add(this.akLabel);
            this.resultGroupBox.Controls.Add(this.ikLabel);
            this.resultGroupBox.Controls.Add(this.ckLabel);
            this.resultGroupBox.Controls.Add(this.resLabel);
            this.resultGroupBox.Controls.Add(this.opcLabel);
            this.resultGroupBox.Controls.Add(this.kcLabel);
            this.resultGroupBox.Controls.Add(this.sresLabel);
            this.resultGroupBox.Controls.Add(this.akText);
            this.resultGroupBox.Controls.Add(this.ikText);
            this.resultGroupBox.Controls.Add(this.ckText);
            this.resultGroupBox.Controls.Add(this.resText);
            this.resultGroupBox.Controls.Add(this.opcText);
            this.resultGroupBox.Controls.Add(this.kcText);
            this.resultGroupBox.Controls.Add(this.sresText);
            this.resultGroupBox.Location = new System.Drawing.Point(32, 191);
            this.resultGroupBox.Name = "resultGroupBox";
            this.resultGroupBox.Size = new System.Drawing.Size(694, 299);
            this.resultGroupBox.TabIndex = 14;
            this.resultGroupBox.TabStop = false;
            // 
            // macLabel
            // 
            this.macLabel.AutoSize = true;
            this.macLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.macLabel.Location = new System.Drawing.Point(20, 229);
            this.macLabel.Name = "macLabel";
            this.macLabel.Size = new System.Drawing.Size(36, 16);
            this.macLabel.TabIndex = 17;
            this.macLabel.Text = "MAC";
            // 
            // macText
            // 
            this.macText.Location = new System.Drawing.Point(107, 223);
            this.macText.Name = "macText";
            this.macText.ReadOnly = true;
            this.macText.Size = new System.Drawing.Size(352, 22);
            this.macText.TabIndex = 16;
            // 
            // autnLabel
            // 
            this.autnLabel.AutoSize = true;
            this.autnLabel.ForeColor = System.Drawing.SystemColors.GrayText;
            this.autnLabel.Location = new System.Drawing.Point(20, 267);
            this.autnLabel.Name = "autnLabel";
            this.autnLabel.Size = new System.Drawing.Size(45, 16);
            this.autnLabel.TabIndex = 15;
            this.autnLabel.Text = "AUTN";
            // 
            // autnText
            // 
            this.autnText.Location = new System.Drawing.Point(106, 263);
            this.autnText.Name = "autnText";
            this.autnText.ReadOnly = true;
            this.autnText.Size = new System.Drawing.Size(352, 22);
            this.autnText.TabIndex = 14;
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
            this.akText.Location = new System.Drawing.Point(107, 195);
            this.akText.Name = "akText";
            this.akText.ReadOnly = true;
            this.akText.Size = new System.Drawing.Size(352, 22);
            this.akText.TabIndex = 6;
            // 
            // ikText
            // 
            this.ikText.Location = new System.Drawing.Point(107, 168);
            this.ikText.Name = "ikText";
            this.ikText.ReadOnly = true;
            this.ikText.Size = new System.Drawing.Size(352, 22);
            this.ikText.TabIndex = 5;
            // 
            // ckText
            // 
            this.ckText.Location = new System.Drawing.Point(107, 141);
            this.ckText.Name = "ckText";
            this.ckText.ReadOnly = true;
            this.ckText.Size = new System.Drawing.Size(352, 22);
            this.ckText.TabIndex = 4;
            // 
            // resText
            // 
            this.resText.Location = new System.Drawing.Point(107, 115);
            this.resText.Name = "resText";
            this.resText.ReadOnly = true;
            this.resText.Size = new System.Drawing.Size(353, 22);
            this.resText.TabIndex = 3;
            // 
            // opcText
            // 
            this.opcText.Location = new System.Drawing.Point(107, 89);
            this.opcText.Name = "opcText";
            this.opcText.ReadOnly = true;
            this.opcText.Size = new System.Drawing.Size(353, 22);
            this.opcText.TabIndex = 2;
            // 
            // kcText
            // 
            this.kcText.Location = new System.Drawing.Point(106, 50);
            this.kcText.Name = "kcText";
            this.kcText.ReadOnly = true;
            this.kcText.Size = new System.Drawing.Size(354, 22);
            this.kcText.TabIndex = 1;
            // 
            // sresText
            // 
            this.sresText.Location = new System.Drawing.Point(105, 24);
            this.sresText.Name = "sresText";
            this.sresText.ReadOnly = true;
            this.sresText.Size = new System.Drawing.Size(355, 22);
            this.sresText.TabIndex = 0;
            // 
            // algoGroupBox
            // 
            this.algoGroupBox.Controls.Add(this.compRadioButton);
            this.algoGroupBox.Controls.Add(this.milenageRadioButton);
            this.algoGroupBox.Location = new System.Drawing.Point(543, 9);
            this.algoGroupBox.Name = "algoGroupBox";
            this.algoGroupBox.Size = new System.Drawing.Size(149, 83);
            this.algoGroupBox.TabIndex = 13;
            this.algoGroupBox.TabStop = false;
            this.algoGroupBox.Text = "Algorithms";
            // 
            // compRadioButton
            // 
            this.compRadioButton.AutoSize = true;
            this.compRadioButton.Checked = true;
            this.compRadioButton.Location = new System.Drawing.Point(6, 21);
            this.compRadioButton.Name = "compRadioButton";
            this.compRadioButton.Size = new System.Drawing.Size(109, 20);
            this.compRadioButton.TabIndex = 11;
            this.compRadioButton.TabStop = true;
            this.compRadioButton.Text = "COMP-128 v3";
            this.compRadioButton.UseVisualStyleBackColor = true;
            this.compRadioButton.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
            // 
            // milenageRadioButton
            // 
            this.milenageRadioButton.AutoSize = true;
            this.milenageRadioButton.Location = new System.Drawing.Point(6, 47);
            this.milenageRadioButton.Name = "milenageRadioButton";
            this.milenageRadioButton.Size = new System.Drawing.Size(84, 20);
            this.milenageRadioButton.TabIndex = 12;
            this.milenageRadioButton.Text = "Milenage";
            this.milenageRadioButton.UseVisualStyleBackColor = true;
            this.milenageRadioButton.CheckedChanged += new System.EventHandler(this.radioButton2_CheckedChanged);
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
            this.tabPage1.Size = new System.Drawing.Size(737, 502);
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
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.Color.WhiteSmoke;
            this.tabPage3.Controls.Add(this.label3);
            this.tabPage3.Controls.Add(this.opText2);
            this.tabPage3.Controls.Add(this.macResultLabel);
            this.tabPage3.Controls.Add(this.errLabel2);
            this.tabPage3.Controls.Add(this.groupBox1);
            this.tabPage3.Controls.Add(this.button4);
            this.tabPage3.Controls.Add(this.label14);
            this.tabPage3.Controls.Add(this.randText2);
            this.tabPage3.Controls.Add(this.label15);
            this.tabPage3.Controls.Add(this.kText2);
            this.tabPage3.Controls.Add(this.autnLabel2);
            this.tabPage3.Controls.Add(this.autnText2);
            this.tabPage3.Location = new System.Drawing.Point(104, 4);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(737, 502);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Verify Milenage";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(47, 110);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(26, 16);
            this.label3.TabIndex = 31;
            this.label3.Text = "OP";
            // 
            // opText2
            // 
            this.opText2.Location = new System.Drawing.Point(134, 109);
            this.opText2.MaxLength = 32;
            this.opText2.Name = "opText2";
            this.opText2.Size = new System.Drawing.Size(353, 22);
            this.opText2.TabIndex = 30;
            // 
            // macResultLabel
            // 
            this.macResultLabel.AutoSize = true;
            this.macResultLabel.Location = new System.Drawing.Point(137, 241);
            this.macResultLabel.Name = "macResultLabel";
            this.macResultLabel.Size = new System.Drawing.Size(0, 16);
            this.macResultLabel.TabIndex = 16;
            // 
            // errLabel2
            // 
            this.errLabel2.AutoSize = true;
            this.errLabel2.Location = new System.Drawing.Point(473, 144);
            this.errLabel2.Name = "errLabel2";
            this.errLabel2.Size = new System.Drawing.Size(0, 16);
            this.errLabel2.TabIndex = 29;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.sqnText2);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.label8);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.akText2);
            this.groupBox1.Controls.Add(this.ikText2);
            this.groupBox1.Controls.Add(this.ckText2);
            this.groupBox1.Controls.Add(this.resText2);
            this.groupBox1.Controls.Add(this.opcText2);
            this.groupBox1.Location = new System.Drawing.Point(27, 211);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(694, 282);
            this.groupBox1.TabIndex = 28;
            this.groupBox1.TabStop = false;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label4.Location = new System.Drawing.Point(20, 222);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(36, 16);
            this.label4.TabIndex = 15;
            this.label4.Text = "SQN";
            // 
            // sqnText2
            // 
            this.sqnText2.Location = new System.Drawing.Point(106, 222);
            this.sqnText2.Name = "sqnText2";
            this.sqnText2.ReadOnly = true;
            this.sqnText2.Size = new System.Drawing.Size(352, 22);
            this.sqnText2.TabIndex = 14;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label5.Location = new System.Drawing.Point(20, 175);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(24, 16);
            this.label5.TabIndex = 13;
            this.label5.Text = "AK";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label6.Location = new System.Drawing.Point(20, 148);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(18, 16);
            this.label6.TabIndex = 12;
            this.label6.Text = "IK";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label7.Location = new System.Drawing.Point(20, 121);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(24, 16);
            this.label7.TabIndex = 11;
            this.label7.Text = "CK";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label8.Location = new System.Drawing.Point(20, 95);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(35, 16);
            this.label8.TabIndex = 10;
            this.label8.Text = "RES";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label9.Location = new System.Drawing.Point(20, 69);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(35, 16);
            this.label9.TabIndex = 9;
            this.label9.Text = "OPC";
            // 
            // akText2
            // 
            this.akText2.Location = new System.Drawing.Point(107, 169);
            this.akText2.Name = "akText2";
            this.akText2.ReadOnly = true;
            this.akText2.Size = new System.Drawing.Size(352, 22);
            this.akText2.TabIndex = 6;
            // 
            // ikText2
            // 
            this.ikText2.Location = new System.Drawing.Point(107, 142);
            this.ikText2.Name = "ikText2";
            this.ikText2.ReadOnly = true;
            this.ikText2.Size = new System.Drawing.Size(352, 22);
            this.ikText2.TabIndex = 5;
            // 
            // ckText2
            // 
            this.ckText2.Location = new System.Drawing.Point(107, 115);
            this.ckText2.Name = "ckText2";
            this.ckText2.ReadOnly = true;
            this.ckText2.Size = new System.Drawing.Size(352, 22);
            this.ckText2.TabIndex = 4;
            // 
            // resText2
            // 
            this.resText2.Location = new System.Drawing.Point(107, 89);
            this.resText2.Name = "resText2";
            this.resText2.ReadOnly = true;
            this.resText2.Size = new System.Drawing.Size(353, 22);
            this.resText2.TabIndex = 3;
            // 
            // opcText2
            // 
            this.opcText2.Location = new System.Drawing.Point(107, 63);
            this.opcText2.Name = "opcText2";
            this.opcText2.ReadOnly = true;
            this.opcText2.Size = new System.Drawing.Size(353, 22);
            this.opcText2.TabIndex = 2;
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(265, 144);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(115, 23);
            this.button4.TabIndex = 16;
            this.button4.Text = "Verify";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(47, 79);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(46, 16);
            this.label14.TabIndex = 22;
            this.label14.Text = "RAND";
            // 
            // randText2
            // 
            this.randText2.Location = new System.Drawing.Point(134, 77);
            this.randText2.MaxLength = 32;
            this.randText2.Name = "randText2";
            this.randText2.Size = new System.Drawing.Size(353, 22);
            this.randText2.TabIndex = 21;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(47, 50);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(15, 16);
            this.label15.TabIndex = 20;
            this.label15.Text = "K";
            // 
            // kText2
            // 
            this.kText2.Location = new System.Drawing.Point(133, 46);
            this.kText2.MaxLength = 32;
            this.kText2.Name = "kText2";
            this.kText2.Size = new System.Drawing.Size(353, 22);
            this.kText2.TabIndex = 19;
            // 
            // autnLabel2
            // 
            this.autnLabel2.AutoSize = true;
            this.autnLabel2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.autnLabel2.Location = new System.Drawing.Point(46, 17);
            this.autnLabel2.Name = "autnLabel2";
            this.autnLabel2.Size = new System.Drawing.Size(45, 16);
            this.autnLabel2.TabIndex = 18;
            this.autnLabel2.Text = "AUTN";
            // 
            // autnText2
            // 
            this.autnText2.BackColor = System.Drawing.SystemColors.Window;
            this.autnText2.Location = new System.Drawing.Point(133, 14);
            this.autnText2.MaxLength = 32;
            this.autnText2.Name = "autnText2";
            this.autnText2.Size = new System.Drawing.Size(353, 22);
            this.autnText2.TabIndex = 17;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(874, 534);
            this.Controls.Add(this.tabControl1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "ConvertToScr";
            this.tabControl1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.resultGroupBox.ResumeLayout(false);
            this.resultGroupBox.PerformLayout();
            this.algoGroupBox.ResumeLayout(false);
            this.algoGroupBox.PerformLayout();
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
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
        private System.Windows.Forms.RadioButton milenageRadioButton;
        private System.Windows.Forms.RadioButton compRadioButton;
        private System.Windows.Forms.GroupBox algoGroupBox;
        private System.Windows.Forms.GroupBox resultGroupBox;
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
        private System.Windows.Forms.Label errorLabel;
        private System.Windows.Forms.Label autnLabel;
        private System.Windows.Forms.TextBox autnText;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.Label errLabel2;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox randText2;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox kText2;
        private System.Windows.Forms.Label autnLabel2;
        private System.Windows.Forms.TextBox autnText2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox sqnText2;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox akText2;
        private System.Windows.Forms.TextBox ikText2;
        private System.Windows.Forms.TextBox ckText2;
        private System.Windows.Forms.TextBox resText2;
        private System.Windows.Forms.TextBox opcText2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox opText2;
        private System.Windows.Forms.Label macResultLabel;
        private System.Windows.Forms.Label macLabel;
        private System.Windows.Forms.TextBox macText;
    }
}

