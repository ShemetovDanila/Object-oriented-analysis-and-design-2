using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace WinFormsApp1
{
    public class PizzaMenuItem
    {
        public string Name { get; set; }
        public int BasePrice { get; set; }
        public string ImageFile { get; set; }

        public string DefaultDough { get; set; }
        public string DefaultSauce { get; set; }

        public int DefaultMozzarella { get; set; }
        public int DefaultCheddar { get; set; }
        public int DefaultPepperoni { get; set; }
        public int DefaultJalapeno { get; set; }

        public bool DefaultChicken { get; set; }
        public bool DefaultMushrooms { get; set; }
        public bool DefaultTomatoes { get; set; }
        public bool DefaultBasil { get; set; }
        public bool DefaultHam { get; set; }
        public bool DefaultPineapple { get; set; }

        public Panel Panel { get; set; }
        public NumericUpDown NumMozzarella { get; set; }
        public NumericUpDown NumCheddar { get; set; }
        public NumericUpDown NumPepperoni { get; set; }
        public NumericUpDown NumJalapeno { get; set; }
        public Button BtnAdd { get; set; }

        public PizzaMenuItem(
            string name,
            int basePrice,
            string imageFile,
            string defaultDough,
            string defaultSauce,
            int defaultMozzarella,
            int defaultCheddar,
            int defaultPepperoni,
            int defaultJalapeno,
            bool defaultChicken,
            bool defaultMushrooms,
            bool defaultTomatoes,
            bool defaultBasil,
            bool defaultHam,
            bool defaultPineapple)
        {
            Name = name;
            BasePrice = basePrice;
            ImageFile = imageFile;
            DefaultDough = defaultDough;
            DefaultSauce = defaultSauce;
            DefaultMozzarella = defaultMozzarella;
            DefaultCheddar = defaultCheddar;
            DefaultPepperoni = defaultPepperoni;
            DefaultJalapeno = defaultJalapeno;
            DefaultChicken = defaultChicken;
            DefaultMushrooms = defaultMushrooms;
            DefaultTomatoes = defaultTomatoes;
            DefaultBasil = defaultBasil;
            DefaultHam = defaultHam;
            DefaultPineapple = defaultPineapple;
        }

        public Panel CreatePanel(EventHandler addButtonClick)
        {
            Panel panel = new Panel();
            panel.Size = new Size(800, 190);
            panel.BorderStyle = BorderStyle.FixedSingle;
            panel.BackColor = Color.White;
            panel.Margin = new Padding(15);
            panel.Padding = new Padding(15);

            PictureBox pictureBox = new PictureBox();
            pictureBox.Size = new Size(160, 160);
            pictureBox.Location = new Point(15, 15);
            pictureBox.SizeMode = PictureBoxSizeMode.Zoom;
            pictureBox.BorderStyle = BorderStyle.FixedSingle;

            string exePath = AppDomain.CurrentDomain.BaseDirectory;
            string imagePath = Path.Combine(exePath, ImageFile);

            if (File.Exists(imagePath))
            {
                Image img = Image.FromFile(imagePath);
                pictureBox.Image = new Bitmap(img, new Size(160, 160));
                img.Dispose();
            }
            else
            {
                pictureBox.BackColor = Color.LightYellow;
                Label lblNoImage = new Label();
                lblNoImage.Text = "Нет фото";
                lblNoImage.AutoSize = true;
                lblNoImage.Location = new Point(50, 65);
                pictureBox.Controls.Add(lblNoImage);
            }

            panel.Controls.Add(pictureBox);

            Label lblName = new Label();
            lblName.Text = Name;
            lblName.Font = new Font("Arial", 20, FontStyle.Bold);
            lblName.Location = new Point(190, 15);
            lblName.AutoSize = true;
            panel.Controls.Add(lblName);

            Label lblBasePrice = new Label();
            lblBasePrice.Text = $"{BasePrice} руб. (база)";
            lblBasePrice.Font = new Font("Arial", 12, FontStyle.Regular);
            lblBasePrice.ForeColor = Color.Gray;
            lblBasePrice.Location = new Point(190, 48);
            lblBasePrice.AutoSize = true;
            panel.Controls.Add(lblBasePrice);

            Label lblToppings = new Label();
            lblToppings.Text = "Добавить топпинги:";
            lblToppings.Font = new Font("Arial", 11, FontStyle.Bold);
            lblToppings.Location = new Point(190, 80);
            lblToppings.AutoSize = true;
            panel.Controls.Add(lblToppings);

            int toppingStartX = 190;
            int toppingStartY = 110;
            int toppingSpacing = 160;

            NumMozzarella = CreateToppingControl(panel, "Моцц (60₽)", toppingStartX, toppingStartY);
            NumCheddar = CreateToppingControl(panel, "Чедд (70₽)", toppingStartX + toppingSpacing, toppingStartY);
            NumPepperoni = CreateToppingControl(panel, "Пепп (80₽)", toppingStartX + toppingSpacing * 2, toppingStartY);
            NumJalapeno = CreateToppingControl(panel, "Хал (50₽)", toppingStartX + toppingSpacing * 3, toppingStartY);

            BtnAdd = new Button();
            BtnAdd.Text = "Добавить в заказ";
            BtnAdd.Size = new Size(180, 50);
            BtnAdd.Location = new Point(600, 70);
            BtnAdd.Font = new Font("Arial", 12, FontStyle.Bold);
            BtnAdd.BackColor = Color.Green;
            BtnAdd.ForeColor = Color.White;
            BtnAdd.FlatStyle = FlatStyle.Flat;
            BtnAdd.FlatAppearance.BorderSize = 0;
            BtnAdd.Click += addButtonClick;
            panel.Controls.Add(BtnAdd);

            Panel = panel;
            return panel;
        }

        private NumericUpDown CreateToppingControl(Panel parent, string name, int x, int y)
        {
            Label lblName = new Label();
            lblName.Text = name;
            lblName.Font = new Font("Arial", 9, FontStyle.Regular);
            lblName.Location = new Point(x, y);
            lblName.AutoSize = true;
            parent.Controls.Add(lblName);

            NumericUpDown num = new NumericUpDown();
            num.Minimum = 0;
            num.Maximum = 10;
            num.Value = 0;
            num.Size = new Size(65, 25);
            num.Location = new Point(x, y + 22);
            num.Font = new Font("Arial", 10);
            parent.Controls.Add(num);

            return num;
        }

        public Pizza CreatePizza()
        {
            return new Pizza(
                Name,
                BasePrice,
                DefaultDough,
                DefaultSauce,
                DefaultMozzarella + (int)NumMozzarella.Value,
                DefaultCheddar + (int)NumCheddar.Value,
                DefaultPepperoni + (int)NumPepperoni.Value,
                DefaultJalapeno + (int)NumJalapeno.Value,
                DefaultMozzarella,
                DefaultCheddar,
                DefaultPepperoni,
                DefaultJalapeno,
                DefaultChicken,
                DefaultMushrooms,
                DefaultTomatoes,
                DefaultBasil,
                DefaultHam,
                DefaultPineapple);
        }
    }
}