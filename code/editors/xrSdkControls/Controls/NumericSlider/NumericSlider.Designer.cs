namespace XRay.SdkControls
{
    partial class NumericSlider
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
            this.numSpinner = new System.Windows.Forms.NumericUpDown();
            ((System.ComponentModel.ISupportInitialize)(this.numSpinner)).BeginInit();
            this.SuspendLayout();
            // 
            // numSpinner
            // 
            this.numSpinner.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.numSpinner.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.numSpinner.Location = new System.Drawing.Point(212, 0);
            this.numSpinner.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numSpinner.MinimumSize = new System.Drawing.Size(32, 0);
            this.numSpinner.Name = "numSpinner";
            this.numSpinner.Size = new System.Drawing.Size(44, 21);
            this.numSpinner.TabIndex = 0;
            // 
            // NumericSlider
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.Controls.Add(this.numSpinner);
            this.MaximumSize = new System.Drawing.Size(9000, 21);
            this.MinimumSize = new System.Drawing.Size(64, 21);
            this.Name = "NumericSlider";
            this.Size = new System.Drawing.Size(256, 21);
            ((System.ComponentModel.ISupportInitialize)(this.numSpinner)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NumericUpDown numSpinner;
    }
}
