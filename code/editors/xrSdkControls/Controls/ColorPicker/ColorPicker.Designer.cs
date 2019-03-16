namespace XRay.SdkControls
{
    partial class ColorPicker
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ColorPicker));
			this.lRed = new System.Windows.Forms.Label();
			this.lGreen = new System.Windows.Forms.Label();
			this.lBlue = new System.Windows.Forms.Label();
			this.lAlpha = new System.Windows.Forms.Label();
			this.chkHexadecimal = new System.Windows.Forms.CheckBox();
			this.tbHexColor = new System.Windows.Forms.TextBox();
			this.lHexColor = new System.Windows.Forms.Label();
			this.panel1 = new System.Windows.Forms.Panel();
			this.isAReversed = new System.Windows.Forms.CheckBox();
			this.label1 = new System.Windows.Forms.Label();
			this.pbColor = new XRay.SdkControls.ColorSampleBox();
			this.nslRed = new XRay.SdkControls.NumericSlider();
			this.nslGreen = new XRay.SdkControls.NumericSlider();
			this.nslBlue = new XRay.SdkControls.NumericSlider();
			this.nslAlpha = new XRay.SdkControls.NumericSlider();
			this.isRReversed = new System.Windows.Forms.CheckBox();
			this.isGReversed = new System.Windows.Forms.CheckBox();
			this.isBReversed = new System.Windows.Forms.CheckBox();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// lRed
			// 
			this.lRed.AutoSize = true;
			this.lRed.Location = new System.Drawing.Point(3, 5);
			this.lRed.Name = "lRed";
			this.lRed.Size = new System.Drawing.Size(15, 13);
			this.lRed.TabIndex = 9;
			this.lRed.Text = "R";
			// 
			// lGreen
			// 
			this.lGreen.AutoSize = true;
			this.lGreen.Location = new System.Drawing.Point(3, 31);
			this.lGreen.Name = "lGreen";
			this.lGreen.Size = new System.Drawing.Size(15, 13);
			this.lGreen.TabIndex = 10;
			this.lGreen.Text = "G";
			// 
			// lBlue
			// 
			this.lBlue.AutoSize = true;
			this.lBlue.Location = new System.Drawing.Point(3, 57);
			this.lBlue.Name = "lBlue";
			this.lBlue.Size = new System.Drawing.Size(14, 13);
			this.lBlue.TabIndex = 11;
			this.lBlue.Text = "B";
			// 
			// lAlpha
			// 
			this.lAlpha.AutoSize = true;
			this.lAlpha.Location = new System.Drawing.Point(3, 83);
			this.lAlpha.Name = "lAlpha";
			this.lAlpha.Size = new System.Drawing.Size(14, 13);
			this.lAlpha.TabIndex = 12;
			this.lAlpha.Text = "A";
			// 
			// chkHexadecimal
			// 
			this.chkHexadecimal.AutoSize = true;
			this.chkHexadecimal.Location = new System.Drawing.Point(3, 120);
			this.chkHexadecimal.Name = "chkHexadecimal";
			this.chkHexadecimal.Size = new System.Drawing.Size(87, 17);
			this.chkHexadecimal.TabIndex = 13;
			this.chkHexadecimal.Text = "Hexadecimal";
			this.chkHexadecimal.UseVisualStyleBackColor = true;
			// 
			// tbHexColor
			// 
			this.tbHexColor.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
			this.tbHexColor.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbHexColor.Location = new System.Drawing.Point(113, 116);
			this.tbHexColor.Margin = new System.Windows.Forms.Padding(0, 3, 3, 3);
			this.tbHexColor.MaxLength = 8;
			this.tbHexColor.Name = "tbHexColor";
			this.tbHexColor.Size = new System.Drawing.Size(80, 21);
			this.tbHexColor.TabIndex = 14;
			this.tbHexColor.Text = "AAAAAAAA";
			this.tbHexColor.Visible = false;
			// 
			// lHexColor
			// 
			this.lHexColor.AutoSize = true;
			this.lHexColor.Location = new System.Drawing.Point(96, 119);
			this.lHexColor.Name = "lHexColor";
			this.lHexColor.Size = new System.Drawing.Size(14, 13);
			this.lHexColor.TabIndex = 15;
			this.lHexColor.Text = "#";
			this.lHexColor.Visible = false;
			// 
			// panel1
			// 
			this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.panel1.Controls.Add(this.isBReversed);
			this.panel1.Controls.Add(this.isGReversed);
			this.panel1.Controls.Add(this.isRReversed);
			this.panel1.Controls.Add(this.isAReversed);
			this.panel1.Controls.Add(this.label1);
			this.panel1.Location = new System.Drawing.Point(220, 106);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(158, 35);
			this.panel1.TabIndex = 20;
			// 
			// isAReversed
			// 
			this.isAReversed.AutoSize = true;
			this.isAReversed.Location = new System.Drawing.Point(38, 14);
			this.isAReversed.Name = "isAReversed";
			this.isAReversed.Size = new System.Drawing.Size(33, 17);
			this.isAReversed.TabIndex = 4;
			this.isAReversed.Text = "A";
			this.isAReversed.UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.BackColor = System.Drawing.Color.Transparent;
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 7F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.label1.Location = new System.Drawing.Point(51, 0);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(91, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "Reverse channels";
			// 
			// pbColor
			// 
			this.pbColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.pbColor.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("pbColor.BackgroundImage")));
			this.pbColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pbColor.ColorSample = System.Drawing.Color.Empty;
			this.pbColor.Location = new System.Drawing.Point(274, 3);
			this.pbColor.Name = "pbColor";
			this.pbColor.Size = new System.Drawing.Size(98, 98);
			this.pbColor.TabIndex = 8;
			this.pbColor.TabStop = false;
			// 
			// nslRed
			// 
			this.nslRed.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.nslRed.BackColor = System.Drawing.SystemColors.Window;
			this.nslRed.Location = new System.Drawing.Point(15, 1);
			this.nslRed.MaximumSize = new System.Drawing.Size(9000, 21);
			this.nslRed.MinimumSize = new System.Drawing.Size(100, 21);
			this.nslRed.Name = "nslRed";
			this.nslRed.Size = new System.Drawing.Size(253, 21);
			this.nslRed.TabIndex = 16;
			this.nslRed.Tag = "red";
			this.nslRed.Value = new decimal(new int[] {
            0,
            0,
            0,
            0});
			// 
			// nslGreen
			// 
			this.nslGreen.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.nslGreen.BackColor = System.Drawing.SystemColors.Window;
			this.nslGreen.Location = new System.Drawing.Point(15, 27);
			this.nslGreen.MaximumSize = new System.Drawing.Size(9000, 21);
			this.nslGreen.MinimumSize = new System.Drawing.Size(100, 21);
			this.nslGreen.Name = "nslGreen";
			this.nslGreen.Size = new System.Drawing.Size(253, 21);
			this.nslGreen.TabIndex = 17;
			this.nslGreen.Tag = "green";
			this.nslGreen.Value = new decimal(new int[] {
            0,
            0,
            0,
            0});
			// 
			// nslBlue
			// 
			this.nslBlue.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.nslBlue.BackColor = System.Drawing.SystemColors.Window;
			this.nslBlue.Location = new System.Drawing.Point(15, 53);
			this.nslBlue.MaximumSize = new System.Drawing.Size(9000, 21);
			this.nslBlue.MinimumSize = new System.Drawing.Size(100, 21);
			this.nslBlue.Name = "nslBlue";
			this.nslBlue.Size = new System.Drawing.Size(253, 21);
			this.nslBlue.TabIndex = 18;
			this.nslBlue.Tag = "blue";
			this.nslBlue.Value = new decimal(new int[] {
            0,
            0,
            0,
            0});
			// 
			// nslAlpha
			// 
			this.nslAlpha.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.nslAlpha.BackColor = System.Drawing.SystemColors.Window;
			this.nslAlpha.Location = new System.Drawing.Point(15, 79);
			this.nslAlpha.MaximumSize = new System.Drawing.Size(9000, 21);
			this.nslAlpha.MinimumSize = new System.Drawing.Size(100, 21);
			this.nslAlpha.Name = "nslAlpha";
			this.nslAlpha.Size = new System.Drawing.Size(253, 21);
			this.nslAlpha.TabIndex = 19;
			this.nslAlpha.Tag = "alpha";
			this.nslAlpha.Value = new decimal(new int[] {
            0,
            0,
            0,
            0});
			// 
			// isRReversed
			// 
			this.isRReversed.AutoSize = true;
			this.isRReversed.BackColor = System.Drawing.Color.Transparent;
			this.isRReversed.Location = new System.Drawing.Point(68, 14);
			this.isRReversed.Name = "isRReversed";
			this.isRReversed.Size = new System.Drawing.Size(34, 17);
			this.isRReversed.TabIndex = 5;
			this.isRReversed.Text = "R";
			this.isRReversed.UseVisualStyleBackColor = false;
			// 
			// isGReversed
			// 
			this.isGReversed.AutoSize = true;
			this.isGReversed.Location = new System.Drawing.Point(98, 14);
			this.isGReversed.Name = "isGReversed";
			this.isGReversed.Size = new System.Drawing.Size(34, 17);
			this.isGReversed.TabIndex = 6;
			this.isGReversed.Text = "G";
			this.isGReversed.UseVisualStyleBackColor = true;
			// 
			// isBReversed
			// 
			this.isBReversed.AutoSize = true;
			this.isBReversed.Location = new System.Drawing.Point(129, 14);
			this.isBReversed.Name = "isBReversed";
			this.isBReversed.Size = new System.Drawing.Size(33, 17);
			this.isBReversed.TabIndex = 7;
			this.isBReversed.Text = "B";
			this.isBReversed.UseVisualStyleBackColor = true;
			// 
			// ColorPicker
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Window;
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.lHexColor);
			this.Controls.Add(this.tbHexColor);
			this.Controls.Add(this.chkHexadecimal);
			this.Controls.Add(this.lAlpha);
			this.Controls.Add(this.lBlue);
			this.Controls.Add(this.lGreen);
			this.Controls.Add(this.lRed);
			this.Controls.Add(this.pbColor);
			this.Controls.Add(this.nslRed);
			this.Controls.Add(this.nslGreen);
			this.Controls.Add(this.nslBlue);
			this.Controls.Add(this.nslAlpha);
			this.MaximumSize = new System.Drawing.Size(9000, 144);
			this.MinimumSize = new System.Drawing.Size(256, 144);
			this.Name = "ColorPicker";
			this.Size = new System.Drawing.Size(381, 144);
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lRed;
        private System.Windows.Forms.Label lGreen;
        private System.Windows.Forms.Label lBlue;
        private System.Windows.Forms.Label lAlpha;
        private XRay.SdkControls.ColorSampleBox pbColor;
        private System.Windows.Forms.CheckBox chkHexadecimal;
        private System.Windows.Forms.Label lHexColor;
        private System.Windows.Forms.TextBox tbHexColor;
        private NumericSlider nslRed;
        private NumericSlider nslGreen;
        private NumericSlider nslBlue;
        private NumericSlider nslAlpha;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Label label1;
		public System.Windows.Forms.CheckBox isAReversed;
		public System.Windows.Forms.CheckBox isRReversed;
		public System.Windows.Forms.CheckBox isBReversed;
		public System.Windows.Forms.CheckBox isGReversed;
	}
}
