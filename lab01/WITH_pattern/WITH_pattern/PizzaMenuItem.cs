using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace WITH_pattern
{
    public class PizzaMenuItem
    {
        public string Name { get; set; }
        public int BasePrice { get; set; }
        public string ImageFile { get; set; }
        public Pizza Pizza { get; set; }
        public bool IsCustomPizza { get; set; }

        public Panel Panel { get; set; }
        public ComboBox ComboDough { get; set; }
        public ComboBox ComboSauce { get; set; }
        public Dictionary<Topping, NumericUpDown> ToppingControls { get; set; }
        public Button BtnAdd { get; set; }
        private Label lblDynamicPrice;

        public PizzaMenuItem(string name, int basePrice, string imageFile, Pizza pizza, bool isCustomPizza = false)
        {
            Name = name;
            BasePrice = basePrice;
            ImageFile = imageFile;
            Pizza = pizza;
            IsCustomPizza = isCustomPizza;
            ToppingControls = new Dictionary<Topping, NumericUpDown>();
        }

        public Panel CreatePanel(EventHandler addButtonClick)
        {
            Panel panel = new Panel();
            panel.Size = new Size(1050, 380);
            panel.BorderStyle = BorderStyle.FixedSingle;
            panel.BackColor = Color.White;
            panel.Margin = new Padding(15);
            panel.Padding = new Padding(15);

            PictureBox pictureBox = new PictureBox();
            pictureBox.Size = new Size(200, 200);
            pictureBox.Location = new Point(15, 15);
            pictureBox.SizeMode = PictureBoxSizeMode.Zoom;
            pictureBox.BorderStyle = BorderStyle.FixedSingle;

            string exePath = AppDomain.CurrentDomain.BaseDirectory;
            string imagePath = Path.Combine(exePath, ImageFile);

            if (File.Exists(imagePath))
            {
                Image img = Image.FromFile(imagePath);
                pictureBox.Image = new Bitmap(img, new Size(200, 200));
                img.Dispose();
            }
            else
            {
                pictureBox.BackColor = Color.LightYellow;
                Label lblNoImage = new Label();
                lblNoImage.Text = "Нет фото";
                lblNoImage.AutoSize = true;
                lblNoImage.Location = new Point(70, 85);
                pictureBox.Controls.Add(lblNoImage);
            }

            panel.Controls.Add(pictureBox);

            Label lblName = new Label();
            lblName.Text = Name;
            lblName.Font = new Font("Arial", 24, FontStyle.Bold);
            lblName.Location = new Point(230, 15);
            lblName.AutoSize = true;
            panel.Controls.Add(lblName);

            Label lblBasePrice = new Label();
            if (IsCustomPizza)
            {
                lblBasePrice.Text = $"База: {BasePrice} руб. + тесто + соус + топпинги";
            }
            else
            {
                lblBasePrice.Text = $"{BasePrice} руб. (доп. топпинги оплачиваются отдельно)";
            }
            lblBasePrice.Font = new Font("Arial", 12, FontStyle.Regular);
            lblBasePrice.ForeColor = Color.Gray;
            lblBasePrice.Location = new Point(230, 75);
            lblBasePrice.AutoSize = true;
            panel.Controls.Add(lblBasePrice);

            int startY = 125;

            if (IsCustomPizza)
            {
                Label lblDough = new Label();
                lblDough.Text = "Тесто:";
                lblDough.Font = new Font("Arial", 11, FontStyle.Bold);
                lblDough.Location = new Point(230, startY);
                lblDough.AutoSize = true;
                panel.Controls.Add(lblDough);

                ComboDough = new ComboBox();
                ComboDough.Size = new Size(200, 23);
                ComboDough.Location = new Point(230, startY + 22);
                ComboDough.DropDownStyle = ComboBoxStyle.DropDownList;
                ComboDough.SelectedIndexChanged += (s, e) => UpdateDynamicPrice();
                foreach (var dough in ToppingCatalog.GetAllDoughs())
                {
                    ComboDough.Items.Add(dough);
                }
                ComboDough.SelectedIndex = 0;
                panel.Controls.Add(ComboDough);

                Label lblSauce = new Label();
                lblSauce.Text = "Соус:";
                lblSauce.Font = new Font("Arial", 11, FontStyle.Bold);
                lblSauce.Location = new Point(460, startY);
                lblSauce.AutoSize = true;
                panel.Controls.Add(lblSauce);

                ComboSauce = new ComboBox();
                ComboSauce.Size = new Size(200, 23);
                ComboSauce.Location = new Point(460, startY + 22);
                ComboSauce.DropDownStyle = ComboBoxStyle.DropDownList;
                ComboSauce.SelectedIndexChanged += (s, e) => UpdateDynamicPrice();
                foreach (var sauce in ToppingCatalog.GetAllSauces())
                {
                    ComboSauce.Items.Add(sauce);
                }
                ComboSauce.SelectedIndex = 0;
                panel.Controls.Add(ComboSauce);

                startY += 60;
            }
            else
            {
                Label lblDefaultConfig = new Label();
                lblDefaultConfig.Text = $"{Pizza.Dough.Name} | {Pizza.Sauce.Name}";
                lblDefaultConfig.Font = new Font("Arial", 10, FontStyle.Regular);
                lblDefaultConfig.ForeColor = Color.DimGray;
                lblDefaultConfig.Location = new Point(230, startY);
                lblDefaultConfig.AutoSize = true;
                panel.Controls.Add(lblDefaultConfig);
                startY += 30;
            }

            Label lblToppings = new Label();
            lblToppings.Text = "Добавить топпинги (все цены за порцию):";
            lblToppings.Font = new Font("Arial", 12, FontStyle.Bold);
            lblToppings.Location = new Point(230, startY);
            lblToppings.AutoSize = true;
            panel.Controls.Add(lblToppings);

            int toppingStartX = 230;
            int toppingStartY = startY + 35;
            int toppingSpacingX = 145;
            int toppingSpacingY = 75;
            int toppingsPerRow = 5;
            int currentTopping = 0;

            foreach (var topping in ToppingCatalog.GetAllToppings())
            {
                int row = currentTopping / toppingsPerRow;
                int col = currentTopping % toppingsPerRow;

                int x = toppingStartX + (col * toppingSpacingX);
                int y = toppingStartY + (row * toppingSpacingY);

                NumericUpDown num = CreateToppingControl(panel, topping.Name, topping.PricePerPortion, x, y);
                num.ValueChanged += (s, e) => UpdateDynamicPrice();
                ToppingControls[topping] = num;

                currentTopping++;
            }

            int panelHeight = toppingStartY + ((ToppingCatalog.GetAllToppings().Count / toppingsPerRow) + 2) * toppingSpacingY - 70;
            panel.Size = new Size(1050, panelHeight);

            BtnAdd = new Button();
            BtnAdd.Text = IsCustomPizza ? "Создать пиццу" : "Добавить в заказ";
            BtnAdd.Size = new Size(250, 60);
            BtnAdd.Location = new Point(780, 15);
            BtnAdd.Font = new Font("Arial", 14, FontStyle.Bold);
            BtnAdd.BackColor = Color.Green;
            BtnAdd.ForeColor = Color.White;
            BtnAdd.FlatStyle = FlatStyle.Flat;
            BtnAdd.FlatAppearance.BorderSize = 0;
            BtnAdd.Click += addButtonClick;
            panel.Controls.Add(BtnAdd);

            lblDynamicPrice = new Label();
            lblDynamicPrice.Text = $"Текущая цена: {CalculateCurrentPrice()} руб.";
            lblDynamicPrice.Font = new Font("Arial", 16, FontStyle.Bold);
            lblDynamicPrice.ForeColor = Color.FromArgb(255, 140, 0);
            lblDynamicPrice.Location = new Point(700, 85);
            lblDynamicPrice.AutoSize = true;
            panel.Controls.Add(lblDynamicPrice);

            Panel = panel;
            return panel;
        }

        private int CalculateCurrentPrice()
        {
            if (IsCustomPizza)
            {
                int price = BasePrice;

                if (ComboDough != null && ComboDough.SelectedItem != null)
                {
                    price += ((Dough)ComboDough.SelectedItem).Price;
                }

                if (ComboSauce != null && ComboSauce.SelectedItem != null)
                {
                    price += ((Sauce)ComboSauce.SelectedItem).Price;
                }

                foreach (var kvp in ToppingControls)
                {
                    price += kvp.Key.PricePerPortion * (int)kvp.Value.Value;
                }

                return price;
            }
            else
            {
                int price = BasePrice;

                foreach (var kvp in ToppingControls)
                {
                    int basePortions = Pizza.BaseToppings.ContainsKey(kvp.Key) ? Pizza.BaseToppings[kvp.Key] : 0;
                    int extraPortions = (int)kvp.Value.Value;

                    if (extraPortions > 0)
                    {
                        price += kvp.Key.PricePerPortion * extraPortions;
                    }
                }

                return price;
            }
        }

        private void UpdateDynamicPrice()
        {
            if (lblDynamicPrice != null)
            {
                lblDynamicPrice.Text = $"Текущая цена: {CalculateCurrentPrice()} руб.";
            }
        }

        private NumericUpDown CreateToppingControl(Panel parent, string name, int price, int x, int y)
        {
            Label lblName = new Label();
            lblName.Text = name;
            lblName.Font = new Font("Arial", 9, FontStyle.Regular);
            lblName.Location = new Point(x, y);
            lblName.AutoSize = true;
            parent.Controls.Add(lblName);

            Label lblPrice = new Label();
            lblPrice.Text = $"{price}₽/порция";
            lblPrice.Font = new Font("Arial", 8, FontStyle.Regular);
            lblPrice.ForeColor = Color.Gray;
            lblPrice.Location = new Point(x, y + 18);
            lblPrice.AutoSize = true;
            parent.Controls.Add(lblPrice);

            NumericUpDown num = new NumericUpDown();
            num.Minimum = 0;
            num.Maximum = 10;
            num.Value = 0;
            num.Size = new Size(65, 23);
            num.Location = new Point(x, y + 38);
            num.Font = new Font("Arial", 10);
            parent.Controls.Add(num);

            return num;
        }

        public Pizza CreatePizzaWithToppings()
        {
            if (IsCustomPizza)
            {
                Dough selectedDough = (Dough)ComboDough.SelectedItem;
                Sauce selectedSauce = (Sauce)ComboSauce.SelectedItem;

                Pizza pizza = new Pizza("Ваша пицца", selectedDough, selectedSauce, BasePrice, isCustomPizza: true);

                foreach (var kvp in ToppingControls)
                {
                    if (kvp.Value.Value > 0)
                    {
                        pizza.AddTopping(kvp.Key, (int)kvp.Value.Value, isBase: false);
                    }
                }

                return pizza;
            }
            else
            {
                Pizza pizza = new Pizza(Pizza.Name, Pizza.Dough, Pizza.Sauce, BasePrice, isCustomPizza: false);

                foreach (var kvp in Pizza.BaseToppings)
                {
                    pizza.AddTopping(kvp.Key, kvp.Value, isBase: true);
                }

                foreach (var kvp in ToppingControls)
                {
                    if (kvp.Value.Value > 0)
                    {
                        pizza.AddTopping(kvp.Key, (int)kvp.Value.Value, isBase: false);
                    }
                }

                return pizza;
            }
        }
    }
}