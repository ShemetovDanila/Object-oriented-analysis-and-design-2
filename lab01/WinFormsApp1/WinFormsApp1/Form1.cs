using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace WinFormsApp1
{
    public class Form1 : Form
    {
        private ListBox listBoxHistory;
        private NumericUpDown numMargarita;
        private NumericUpDown numPepperoni;
        private NumericUpDown numCountry;
        private Label lblTotalSum;

        private const int PriceMargarita = 450;
        private const int PricePepperoni = 550;
        private const int PriceCountry = 500;

        private const string DoughStandard = "Классическое";
        private const string SauceTomato = "Томатный";
        private const string SauceCream = "Сливочный";

        public Form1()
        {
            this.Text = "Pizza Delivery";
            this.Size = new Size(1200, 750);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.MinimumSize = new Size(1000, 600);

            listBoxHistory = new ListBox();
            numMargarita = new NumericUpDown();
            numPepperoni = new NumericUpDown();
            numCountry = new NumericUpDown();
            lblTotalSum = new Label();

            this.InitializeComponentsManual();
        }

        private void InitializeComponentsManual()
        {
            Panel panelLeft = new Panel();
            panelLeft.Location = new Point(0, 0);
            panelLeft.Size = new Size(300, this.ClientSize.Height);
            panelLeft.BackColor = Color.LightGray;
            panelLeft.Dock = DockStyle.Left;
            this.Controls.Add(panelLeft);

            Label lblHistoryTitle = new Label();
            lblHistoryTitle.Text = "История ПИЦЦ";
            lblHistoryTitle.Font = new Font("Arial", 16, FontStyle.Bold);
            lblHistoryTitle.Location = new Point(15, 15);
            lblHistoryTitle.AutoSize = true;
            panelLeft.Controls.Add(lblHistoryTitle);

            listBoxHistory.Location = new Point(15, 60);
            listBoxHistory.Size = new Size(270, 550);
            listBoxHistory.Font = new Font("Arial", 11);
            listBoxHistory.HorizontalScrollbar = true;
            panelLeft.Controls.Add(listBoxHistory);

            Panel panelRight = new Panel();
            panelRight.Dock = DockStyle.Fill;
            panelRight.Padding = new Padding(40);
            this.Controls.Add(panelRight);

            Label lblTitle = new Label();
            lblTitle.Text = "Оформить заказ";
            lblTitle.Font = new Font("Arial", 20, FontStyle.Bold);
            lblTitle.Location = new Point(40, 30);
            panelRight.Controls.Add(lblTitle);

            CreatePizzaControl(panelRight, "Маргарита", 400, 120, PriceMargarita, "marg.png", out numMargarita);
            CreatePizzaControl(panelRight, "Пепперони", 750, 120, PricePepperoni, "pep.png", out numPepperoni);
            CreatePizzaControl(panelRight, "Кантри", 400, 420, PriceCountry, "kant.png", out numCountry);

            lblTotalSum.Text = "Итого: 0 руб.";
            lblTotalSum.Font = new Font("Arial", 18, FontStyle.Bold);
            lblTotalSum.ForeColor = Color.Black;
            lblTotalSum.AutoSize = true;
            lblTotalSum.Location = new Point(750, 480);
            panelRight.Controls.Add(lblTotalSum);

            Button btnOrder = new Button();
            btnOrder.Text = "Оформить заказ!";
            btnOrder.Size = new Size(250, 70);
            btnOrder.Location = new Point(750, 540);
            btnOrder.Font = new Font("Arial", 14, FontStyle.Bold);
            btnOrder.BackColor = Color.Green;
            btnOrder.ForeColor = Color.White;
            btnOrder.FlatStyle = FlatStyle.Flat;
            btnOrder.FlatAppearance.BorderSize = 0;
            btnOrder.Click += BtnOrder_Click;
            panelRight.Controls.Add(btnOrder);

            numMargarita.ValueChanged += (s, e) => UpdateTotalSum();
            numPepperoni.ValueChanged += (s, e) => UpdateTotalSum();
            numCountry.ValueChanged += (s, e) => UpdateTotalSum();
        }

        private void CreatePizzaControl(Panel parent, string name, int x, int y, int price, string imageName, out NumericUpDown numControl)
        {
            PictureBox pictureBox = new PictureBox();
            pictureBox.Size = new Size(150, 150);
            pictureBox.Location = new Point(x, y);
            pictureBox.SizeMode = PictureBoxSizeMode.Zoom;
            pictureBox.BackColor = Color.White;
            pictureBox.BorderStyle = BorderStyle.FixedSingle;

            string exePath = AppDomain.CurrentDomain.BaseDirectory;
            string imagePath = Path.Combine(exePath, imageName);

            if (File.Exists(imagePath))
            {
                Image img = Image.FromFile(imagePath);
                pictureBox.Image = new Bitmap(img, new Size(150, 150));
                img.Dispose();
            }
            else
            {
                pictureBox.BackColor = Color.LightYellow;
                Label lblError = new Label();
                lblError.Text = "Нет фото";
                lblError.AutoSize = true;
                lblError.Location = new Point(40, 60);
                pictureBox.Controls.Add(lblError);
            }

            parent.Controls.Add(pictureBox);

            Label lblName = new Label();
            lblName.Text = name;
            lblName.Font = new Font("Arial", 14, FontStyle.Bold);
            lblName.AutoSize = true;
            lblName.Location = new Point(x + 15, y + 160);
            parent.Controls.Add(lblName);

            Label lblPrice = new Label();
            lblPrice.Text = $"{price} руб.";
            lblPrice.Font = new Font("Arial", 12, FontStyle.Regular);
            lblPrice.ForeColor = Color.Gray;
            lblPrice.AutoSize = true;
            lblPrice.Location = new Point(x + 40, y + 190);
            parent.Controls.Add(lblPrice);

            numControl = new NumericUpDown();
            numControl.Minimum = 0;
            numControl.Maximum = 99;
            numControl.Value = 0;
            numControl.Size = new Size(120, 30);
            numControl.Location = new Point(x + 15, y + 225);
            numControl.Font = new Font("Arial", 12);
            numControl.Tag = price;
            parent.Controls.Add(numControl);
        }

        private void UpdateTotalSum()
        {
            int total = 0;
            total += (int)numMargarita.Value * (int)numMargarita.Tag;
            total += (int)numPepperoni.Value * (int)numPepperoni.Tag;
            total += (int)numCountry.Value * (int)numCountry.Tag;

            lblTotalSum.Text = $"Итого: {total} руб.";
        }

        private void BtnOrder_Click(object sender, EventArgs e)
        {
            int m = (int)numMargarita.Value;
            int p = (int)numPepperoni.Value;
            int c = (int)numCountry.Value;

            if (m == 0 && p == 0 && c == 0)
            {
                MessageBox.Show("Вы ничего не выбрали! Добавьте пиццы в заказ.");
                return;
            }

            Order order = new Order();

            for (int i = 0; i < m; i++)
            {
                Pizza margarita = new Pizza("Маргарита", PriceMargarita, DoughStandard, SauceTomato);
                order.AddPizza(margarita);
            }

            for (int i = 0; i < p; i++)
            {
                Pizza pepperoni = new Pizza("Пепперони", PricePepperoni, DoughStandard, SauceTomato);
                order.AddPizza(pepperoni);
            }

            for (int i = 0; i < c; i++)
            {
                Pizza country = new Pizza("Кантри", PriceCountry, DoughStandard, SauceCream);
                order.AddPizza(country);
            }

            listBoxHistory.Items.Insert(0, order.GetOrderDescription());

            numMargarita.Value = 0;
            numPepperoni.Value = 0;
            numCountry.Value = 0;
        }
    }
}