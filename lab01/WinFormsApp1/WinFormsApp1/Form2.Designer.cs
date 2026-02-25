using System;
using System.Drawing;
using System.Windows.Forms;

namespace WinFormsApp1
{
    public partial class Form2 : Form
    {
        private ListBox listBoxHistory;
        private Label lblTitle;
        private Button btnClear;

        public Form2()
        {
            this.Text = "История заказов";
            this.Size = new Size(500, 700);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.MinimumSize = new Size(450, 600);

            listBoxHistory = new ListBox();
            lblTitle = new Label();
            btnClear = new Button();

            this.InitializeComponents();
        }

        private void InitializeComponents()
        {
            lblTitle.Text = "История заказов";
            lblTitle.Font = new Font("Arial", 18, FontStyle.Bold);
            lblTitle.Location = new Point(20, 20);
            lblTitle.AutoSize = true;
            this.Controls.Add(lblTitle);

            listBoxHistory.Location = new Point(20, 60);
            listBoxHistory.Size = new Size(440, 550);
            listBoxHistory.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            listBoxHistory.Font = new Font("Arial", 10);
            listBoxHistory.HorizontalScrollbar = true;
            listBoxHistory.BackColor = Color.White;
            listBoxHistory.BorderStyle = BorderStyle.FixedSingle;
            this.Controls.Add(listBoxHistory);

            btnClear.Text = "Очистить историю";
            btnClear.Size = new Size(200, 40);
            btnClear.Location = new Point(20, 620);
            btnClear.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            btnClear.Font = new Font("Arial", 11, FontStyle.Regular);
            btnClear.BackColor = Color.Gray;
            btnClear.ForeColor = Color.White;
            btnClear.FlatStyle = FlatStyle.Flat;
            btnClear.FlatAppearance.BorderSize = 0;
            btnClear.Click += BtnClear_Click;
            this.Controls.Add(btnClear);
        }

        public void AddOrderToHistory(string orderText)
        {
            listBoxHistory.Items.Insert(0, orderText);
        }

        private void BtnClear_Click(object? sender, EventArgs? e)
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
                }
            }
        }
    }
}