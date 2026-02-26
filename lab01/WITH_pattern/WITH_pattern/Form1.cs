using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace WITH_pattern
{
    public partial class Form1 : Form
    {
        private Label lblTotalSum;
        private Label lblTotalCount;
        private FlowLayoutPanel panelMenu;
        private FlowLayoutPanel panelCurrentOrder;
        private Order currentOrder;
        private Button btnCheckout;
        private Button btnHistory;
        private Form2 historyForm;
        private List<string> orderHistoryList;

        private PizzaDirector _director;

        private PizzaMenuItem itemCustom;
        private PizzaMenuItem itemMargarita;
        private PizzaMenuItem itemPepperoni;
        private PizzaMenuItem itemCountry;
        private PizzaMenuItem itemHawaiian;
        private PizzaMenuItem itemChicken;

        public Form1()
        {
            this.Text = "Pizza Delivery";
            this.Size = new Size(1600, 900);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.MinimumSize = new Size(1400, 800);

            lblTotalSum = new Label();
            lblTotalCount = new Label();
            panelMenu = new FlowLayoutPanel();
            panelCurrentOrder = new FlowLayoutPanel();
            currentOrder = new Order();
            historyForm = null;
            orderHistoryList = new List<string>();

            _director = new PizzaDirector();

            this.InitializeComponentsManual();
        }

        public void ClearHistory()
        {
            orderHistoryList.Clear();
        }

        private void InitializeComponentsManual()
        {
            // === ПАНЕЛЬ СПРАВА (Текущий заказ) ===
            Panel panelRight = new Panel();
            panelRight.BackColor = Color.FromArgb(255, 245, 200);
            panelRight.Dock = DockStyle.Right;
            panelRight.Width = 400;
            panelRight.MinimumSize = new Size(400, 0);
            panelRight.Padding = new Padding(0);
            this.Controls.Add(panelRight);

            Label lblOrderTitle = new Label();
            lblOrderTitle.Text = "Текущий заказ";
            lblOrderTitle.Font = new Font("Arial", 18, FontStyle.Bold);
            lblOrderTitle.BackColor = Color.FromArgb(255, 245, 200);
            lblOrderTitle.Padding = new Padding(15, 15, 15, 10);
            lblOrderTitle.Dock = DockStyle.Top;
            lblOrderTitle.Height = 55;
            panelRight.Controls.Add(lblOrderTitle);

            panelCurrentOrder.Dock = DockStyle.Fill;
            panelCurrentOrder.AutoScroll = true;
            panelCurrentOrder.FlowDirection = FlowDirection.TopDown;
            panelCurrentOrder.WrapContents = false;
            panelCurrentOrder.Padding = new Padding(15, 65, 15, 15);
            panelRight.Controls.Add(panelCurrentOrder);

            Panel panelBottomRight = new Panel();
            panelBottomRight.Dock = DockStyle.Bottom;
            panelBottomRight.Height = 230;
            panelBottomRight.BackColor = Color.FromArgb(255, 245, 200);
            panelBottomRight.Padding = new Padding(15);
            panelRight.Controls.Add(panelBottomRight);

            lblTotalCount.Text = "Всего пицц: 0";
            lblTotalCount.Font = new Font("Arial", 13, FontStyle.Bold);
            lblTotalCount.Location = new Point(15, 15);
            lblTotalCount.AutoSize = true;
            panelBottomRight.Controls.Add(lblTotalCount);

            lblTotalSum.Text = "Итого: 0 руб.";
            lblTotalSum.Font = new Font("Arial", 18, FontStyle.Bold);
            lblTotalSum.ForeColor = Color.Black;
            lblTotalSum.Location = new Point(15, 50);
            lblTotalSum.AutoSize = true;
            panelBottomRight.Controls.Add(lblTotalSum);

            btnCheckout = new Button();
            btnCheckout.Text = "Оформить заказ";
            btnCheckout.Size = new Size(370, 50);
            btnCheckout.Location = new Point(15, 100);
            btnCheckout.Font = new Font("Arial", 14, FontStyle.Bold);
            btnCheckout.BackColor = Color.FromArgb(255, 140, 0);
            btnCheckout.ForeColor = Color.White;
            btnCheckout.FlatStyle = FlatStyle.Flat;
            btnCheckout.FlatAppearance.BorderSize = 0;
            btnCheckout.Click += BtnCheckout_Click;
            panelBottomRight.Controls.Add(btnCheckout);

            btnHistory = new Button();
            btnHistory.Text = "📋 История заказов";
            btnHistory.Size = new Size(370, 45);
            btnHistory.Location = new Point(15, 165);
            btnHistory.Font = new Font("Arial", 12, FontStyle.Regular);
            btnHistory.BackColor = Color.FromArgb(100, 100, 100);
            btnHistory.ForeColor = Color.White;
            btnHistory.FlatStyle = FlatStyle.Flat;
            btnHistory.FlatAppearance.BorderSize = 0;
            btnHistory.Click += BtnHistory_Click;
            panelBottomRight.Controls.Add(btnHistory);

            // === ЦЕНТРАЛЬНАЯ ПАНЕЛЬ (Меню) ===
            Panel panelCenter = new Panel();
            panelCenter.Dock = DockStyle.Fill;
            panelCenter.BackColor = Color.White;
            panelCenter.AutoScroll = true;
            panelCenter.Padding = new Padding(40, 30, 40, 30);
            this.Controls.Add(panelCenter);

            Label lblTitle = new Label();
            lblTitle.Text = "Меню пицц";
            lblTitle.Font = new Font("Arial", 28, FontStyle.Bold);
            lblTitle.Location = new Point(40, 30);
            lblTitle.AutoSize = true;
            panelCenter.Controls.Add(lblTitle);

            panelMenu.Location = new Point(40, 90);
            panelMenu.Size = new Size(panelCenter.Width - 80, panelCenter.Height - 120);
            panelMenu.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            panelMenu.AutoScroll = true;
            panelMenu.FlowDirection = FlowDirection.TopDown;
            panelMenu.WrapContents = false;
            panelMenu.Padding = new Padding(15);
            panelCenter.Controls.Add(panelMenu);

            var defaultDough = ToppingCatalog.GetDoughByName("Классическое");
            var defaultSauce = ToppingCatalog.GetSauceByName("Томатный");

            // === СОЗДАНИЕ ПИЦЦ ЧЕРЕЗ BUILDER PATTERN ===
            itemCustom = new PizzaMenuItem("🍕 Конструктор", 200, "myPizza.png",
                new Pizza("Ваша пицца", defaultDough, defaultSauce, 200, true), true);

            var builderMargarita = new PizzaBuilder();
            _director.MakeMargarita(builderMargarita);
            Pizza pizzaMargarita = builderMargarita.GetResult();
            itemMargarita = new PizzaMenuItem("Маргарита", 450, "marg.png", pizzaMargarita, false);

            var builderPepperoni = new PizzaBuilder();
            _director.MakePepperoni(builderPepperoni);
            Pizza pizzaPepperoni = builderPepperoni.GetResult();
            itemPepperoni = new PizzaMenuItem("Пепперони", 550, "pep.png", pizzaPepperoni, false);

            var builderCountry = new PizzaBuilder();
            _director.MakeCountry(builderCountry);
            Pizza pizzaCountry = builderCountry.GetResult();
            itemCountry = new PizzaMenuItem("Кантри", 500, "kant.png", pizzaCountry, false);

            var builderHawaiian = new PizzaBuilder();
            _director.MakeHawaiian(builderHawaiian);
            Pizza pizzaHawaiian = builderHawaiian.GetResult();
            itemHawaiian = new PizzaMenuItem("Гавайская", 520, "pineapple.png", pizzaHawaiian, false);

            var builderChicken = new PizzaBuilder();
            _director.MakeChicken(builderChicken);
            Pizza pizzaChicken = builderChicken.GetResult();
            itemChicken = new PizzaMenuItem("Куриная", 530, "chicken.png", pizzaChicken, false);

            panelMenu.Controls.Add(itemCustom.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemMargarita.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemPepperoni.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemCountry.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemHawaiian.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemChicken.CreatePanel(BtnAdd_Click));
        }

        private void BtnHistory_Click(object sender, EventArgs e)
        {
            if (historyForm == null || historyForm.IsDisposed)
            {
                historyForm = new Form2(this);

                foreach (string order in orderHistoryList)
                {
                    historyForm.AddOrderToHistory(order);
                }

                historyForm.Show();
            }
            else
            {
                historyForm.WindowState = FormWindowState.Normal;
                historyForm.BringToFront();
            }
        }

        private void BtnAdd_Click(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            Panel panel = (Panel)btn.Parent;
            PizzaMenuItem item = null;

            if (panel == itemCustom.Panel) item = itemCustom;
            else if (panel == itemMargarita.Panel) item = itemMargarita;
            else if (panel == itemPepperoni.Panel) item = itemPepperoni;
            else if (panel == itemCountry.Panel) item = itemCountry;
            else if (panel == itemHawaiian.Panel) item = itemHawaiian;
            else if (panel == itemChicken.Panel) item = itemChicken;

            if (item != null)
            {
                Pizza pizza = item.CreatePizzaWithToppings();
                currentOrder.AddPizza(pizza);
                UpdateOrderDisplay();

                foreach (var kvp in item.ToppingControls)
                {
                    kvp.Value.Value = 0;
                }
            }
        }

        private void UpdateOrderDisplay()
        {
            panelCurrentOrder.Controls.Clear();

            foreach (OrderItem orderItem in currentOrder.Items)
            {
                Panel itemPanel = CreateOrderItemPanel(orderItem);
                panelCurrentOrder.Controls.Add(itemPanel);
            }

            lblTotalSum.Text = $"Итого: {currentOrder.GetTotalSum()} руб.";
            lblTotalCount.Text = $"Всего пицц: {currentOrder.GetTotalPizzasCount()}";
        }

        private Panel CreateOrderItemPanel(OrderItem orderItem)
        {
            Panel panel = new Panel();
            panel.Size = new Size(360, 120);
            panel.BackColor = Color.White;
            panel.BorderStyle = BorderStyle.FixedSingle;
            panel.Margin = new Padding(5);
            panel.Padding = new Padding(10);

            Label lblName = new Label();
            lblName.Text = orderItem.Pizza.Name;
            lblName.Font = new Font("Arial", 12, FontStyle.Bold);
            lblName.Location = new Point(10, 10);
            lblName.AutoSize = true;
            panel.Controls.Add(lblName);

            Panel quantityPanel = new Panel();
            quantityPanel.Size = new Size(102, 32);
            quantityPanel.Location = new Point(240, 10);
            quantityPanel.BackColor = Color.White;
            quantityPanel.BorderStyle = BorderStyle.FixedSingle;
            panel.Controls.Add(quantityPanel);

            Button btnMinus = new Button();
            btnMinus.Text = "−";
            btnMinus.Size = new Size(30, 30);
            btnMinus.Location = new Point(1, 1);
            btnMinus.Font = new Font("Arial", 16, FontStyle.Bold);
            btnMinus.BackColor = Color.White;
            btnMinus.ForeColor = Color.Black;
            btnMinus.FlatStyle = FlatStyle.Flat;
            btnMinus.FlatAppearance.BorderSize = 0;
            btnMinus.Click += (s, ev) => DecreaseQuantity_Click(orderItem);
            quantityPanel.Controls.Add(btnMinus);

            Button btnPlus = new Button();
            btnPlus.Text = "+";
            btnPlus.Size = new Size(30, 30);
            btnPlus.Location = new Point(69, 1);
            btnPlus.Font = new Font("Arial", 16, FontStyle.Bold);
            btnPlus.BackColor = Color.White;
            btnPlus.ForeColor = Color.Black;
            btnPlus.FlatStyle = FlatStyle.Flat;
            btnPlus.FlatAppearance.BorderSize = 0;
            btnPlus.Click += (s, ev) => IncreaseQuantity_Click(orderItem);
            quantityPanel.Controls.Add(btnPlus);

            orderItem.lblQuantity = new Label();
            orderItem.lblQuantity.Text = $"{orderItem.Quantity}";
            orderItem.lblQuantity.Font = new Font("Arial", 13, FontStyle.Bold);
            orderItem.lblQuantity.ForeColor = Color.Black;
            orderItem.lblQuantity.Location = new Point(31, 4);
            orderItem.lblQuantity.Size = new Size(38, 26);
            orderItem.lblQuantity.TextAlign = ContentAlignment.MiddleCenter;
            orderItem.lblQuantity.AutoSize = false;
            quantityPanel.Controls.Add(orderItem.lblQuantity);

            Label lblDoughSauce = new Label();
            lblDoughSauce.Text = $"{orderItem.Pizza.Dough.Name} | {orderItem.Pizza.Sauce.Name}";
            lblDoughSauce.Font = new Font("Arial", 8, FontStyle.Regular);
            lblDoughSauce.ForeColor = Color.Gray;
            lblDoughSauce.Location = new Point(10, 38);
            lblDoughSauce.AutoSize = true;
            panel.Controls.Add(lblDoughSauce);

            Label lblToppings = new Label();
            lblToppings.Text = orderItem.Pizza.GetToppingsString();
            lblToppings.Font = new Font("Arial", 8, FontStyle.Regular);
            lblToppings.ForeColor = Color.DimGray;
            lblToppings.Location = new Point(10, 58);
            lblToppings.AutoSize = true;
            lblToppings.MaximumSize = new Size(260, 50);
            panel.Controls.Add(lblToppings);

            Label lblPrice = new Label();
            lblPrice.Text = $"{orderItem.GetTotalPrice()} руб.";
            lblPrice.Font = new Font("Arial", 11, FontStyle.Bold);
            lblPrice.Location = new Point(10, 90);
            lblPrice.AutoSize = true;
            panel.Controls.Add(lblPrice);

            orderItem.Panel = panel;
            return panel;
        }

        private void DecreaseQuantity_Click(OrderItem orderItem)
        {
            if (orderItem.Quantity > 1)
            {
                orderItem.Quantity--;
                orderItem.UpdateQuantityDisplay();
                UpdateOrderDisplay();
            }
            else
            {
                currentOrder.RemoveItem(orderItem);
                UpdateOrderDisplay();
            }
        }

        private void IncreaseQuantity_Click(OrderItem orderItem)
        {
            if (orderItem.Quantity < 99)
            {
                orderItem.Quantity++;
                orderItem.UpdateQuantityDisplay();
                UpdateOrderDisplay();
            }
            else
            {
                MessageBox.Show("Максимум 99 пицц одного вида!", "Лимит", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void RemoveItem_Click(OrderItem orderItem)
        {
            currentOrder.RemoveItem(orderItem);
            UpdateOrderDisplay();
        }

        private void BtnCheckout_Click(object sender, EventArgs e)
        {
            if (currentOrder.Items.Count == 0)
            {
                MessageBox.Show("Добавьте пиццы в заказ!");
                return;
            }

            string historyEntry = $"{DateTime.Now.ToShortTimeString()} - Заказ на {currentOrder.GetTotalSum()} руб. ({currentOrder.GetTotalPizzasCount()} пицц)\n";

            foreach (OrderItem item in currentOrder.Items)
            {
                historyEntry += $"  • {item.Quantity}x {item.Pizza.Name}";
                historyEntry += $" [{item.Pizza.Dough.Name}, {item.Pizza.Sauce.Name}]";
                string extras = item.Pizza.GetToppingsString();
                if (extras != "Без топпингов")
                {
                    historyEntry += $" | {extras}";
                }
                historyEntry += "\n";
            }

            orderHistoryList.Add(historyEntry);

            if (historyForm != null && !historyForm.IsDisposed)
            {
                historyForm.AddOrderToHistory(historyEntry);
            }

            string orderText = $"Заказ оформлен!\nВсего пицц: {currentOrder.GetTotalPizzasCount()}\n";
            orderText += $"Сумма: {currentOrder.GetTotalSum()} руб.\n\n";

            foreach (OrderItem item in currentOrder.Items)
            {
                orderText += $"{item.Quantity}x {item.Pizza.Name} - {item.GetTotalPrice()} руб.\n";
            }

            MessageBox.Show(orderText, "Заказ успешно оформлен!", MessageBoxButtons.OK, MessageBoxIcon.Information);

            currentOrder = new Order();
            UpdateOrderDisplay();
        }
    }
}