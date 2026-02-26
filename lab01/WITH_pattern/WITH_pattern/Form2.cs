using System;
using System.Drawing;
using System.Windows.Forms;

namespace WITH_pattern
{
    public partial class Form2 : Form
    {
        private ListBox listBoxHistory;
        private Label lblTitle;
        private Button btnClear;
        private Form1 parentForm;

        public Form2(Form1 form1)
        {
            this.Text = "История заказов";
            this.Size = new Size(500, 700);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.MinimumSize = new Size(450, 600);
            this.parentForm = form1;

            InitializeComponent();
        }

        private void InitializeComponent()
        {
            this.listBoxHistory = new System.Windows.Forms.ListBox();
            this.lblTitle = new System.Windows.Forms.Label();
            this.btnClear = new System.Windows.Forms.Button();
            this.SuspendLayout();

            this.lblTitle.Text = "История заказов";
            this.lblTitle.Font = new System.Drawing.Font("Arial", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.lblTitle.Location = new System.Drawing.Point(20, 20);
            this.lblTitle.AutoSize = true;
            this.Controls.Add(this.lblTitle);

            this.listBoxHistory.Location = new System.Drawing.Point(20, 60);
            this.listBoxHistory.Size = new System.Drawing.Size(440, 550);
            this.listBoxHistory.Anchor = System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right;
            this.listBoxHistory.Font = new System.Drawing.Font("Arial", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.listBoxHistory.HorizontalScrollbar = true;
            this.listBoxHistory.BackColor = System.Drawing.Color.White;
            this.listBoxHistory.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Controls.Add(this.listBoxHistory);

            this.btnClear.Text = "Очистить историю";
            this.btnClear.Size = new System.Drawing.Size(200, 40);
            this.btnClear.Location = new System.Drawing.Point(20, 620);
            this.btnClear.Anchor = System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left;
            this.btnClear.Font = new System.Drawing.Font("Arial", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.btnClear.BackColor = System.Drawing.Color.Gray;
            this.btnClear.ForeColor = System.Drawing.Color.White;
            this.btnClear.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnClear.FlatAppearance.BorderSize = 0;
            this.btnClear.Click += new System.EventHandler(this.BtnClear_Click);
            this.Controls.Add(this.btnClear);

            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(255, 245, 200);
            this.ClientSize = new System.Drawing.Size(500, 700);
            this.Controls.Add(this.listBoxHistory);
            this.Controls.Add(this.lblTitle);
            this.Controls.Add(this.btnClear);
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        public void AddOrderToHistory(string orderText)
        {
            listBoxHistory.Items.Insert(0, orderText);
        }

        private void BtnClear_Click(object sender, EventArgs e)
        {
            if (listBoxHistory.Items.Count > 0)
            {
                DialogResult result = MessageBox.Show(
                    "Вы уверены, что хотите очистить историю?",
                    "Подтверждение",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Question);

                if (result == DialogResult.Yes)
                {
                    listBoxHistory.Items.Clear();
                    if (parentForm != null)
                    {
                        parentForm.ClearHistory();
                    }
                }
            }
        }
    }
}