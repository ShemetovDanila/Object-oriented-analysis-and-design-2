using System;
using System.Drawing;
using System.Windows.Forms;

namespace WinFormsApp1
{
    public class Form1 : Form
    {
        private ListBox listBoxHistory;
        private Label lblTotalSum;
        private Label lblTotalCount;
        private FlowLayoutPanel panelMenu;
        private FlowLayoutPanel panelCurrentOrder;
        private Order currentOrder;

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

            listBoxHistory = new ListBox();
            lblTotalSum = new Label();
            lblTotalCount = new Label();
            panelMenu = new FlowLayoutPanel();
            panelCurrentOrder = new FlowLayoutPanel();
            currentOrder = new Order();

            this.InitializeComponentsManual();
        }

        private void InitializeComponentsManual()
        {
            Panel panelLeft = new Panel();
            panelLeft.Location = new Point(0, 0);
            panelLeft.Size = new Size(320, 0);
            panelLeft.BackColor = Color.LightGray;
            panelLeft.Dock = DockStyle.Left;
            this.Controls.Add(panelLeft);

            Label lblHistoryTitle = new Label();
            lblHistoryTitle.Text = "История заказов";
            lblHistoryTitle.Font = new Font("Arial", 16, FontStyle.Bold);
            lblHistoryTitle.Location = new Point(15, 15);
            lblHistoryTitle.AutoSize = true;
            panelLeft.Controls.Add(lblHistoryTitle);

            listBoxHistory.Location = new Point(15, 50);
            listBoxHistory.Size = new Size(290, 0);
            listBoxHistory.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            listBoxHistory.Font = new Font("Arial", 9);
            listBoxHistory.HorizontalScrollbar = true;
            panelLeft.Controls.Add(listBoxHistory);

            Panel panelRight = new Panel();
            panelRight.Location = new Point(0, 0);
            panelRight.Size = new Size(400, 0);
            panelRight.BackColor = Color.FromArgb(255, 245, 200);
            panelRight.Dock = DockStyle.Right;
            this.Controls.Add(panelRight);

            Label lblOrderTitle = new Label();
            lblOrderTitle.Text = "Текущий заказ";
            lblOrderTitle.Font = new Font("Arial", 18, FontStyle.Bold);
            lblOrderTitle.Location = new Point(20, 20);
            lblOrderTitle.AutoSize = true;
            panelRight.Controls.Add(lblOrderTitle);

            panelCurrentOrder.Location = new Point(15, 60);
            panelCurrentOrder.Size = new Size(370, 0);
            panelCurrentOrder.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            panelCurrentOrder.AutoScroll = true;
            panelCurrentOrder.FlowDirection = FlowDirection.TopDown;
            panelCurrentOrder.WrapContents = false;
            panelRight.Controls.Add(panelCurrentOrder);

            lblTotalCount.Text = "Всего пицц: 0";
            lblTotalCount.Font = new Font("Arial", 13, FontStyle.Bold);
            lblTotalCount.Location = new Point(20, 0);
            lblTotalCount.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            lblTotalCount.AutoSize = true;
            panelRight.Controls.Add(lblTotalCount);

            lblTotalSum.Text = "Итого: 0 руб.";
            lblTotalSum.Font = new Font("Arial", 18, FontStyle.Bold);
            lblTotalSum.ForeColor = Color.Black;
            lblTotalSum.Location = new Point(20, 0);
            lblTotalSum.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            lblTotalSum.AutoSize = true;
            panelRight.Controls.Add(lblTotalSum);

            Button btnCheckout = new Button();
            btnCheckout.Text = "Оформить заказ";
            btnCheckout.Size = new Size(360, 55);
            btnCheckout.Location = new Point(20, 0);
            btnCheckout.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            btnCheckout.Font = new Font("Arial", 14, FontStyle.Bold);
            btnCheckout.BackColor = Color.FromArgb(255, 140, 0);
            btnCheckout.ForeColor = Color.White;
            btnCheckout.FlatStyle = FlatStyle.Flat;
            btnCheckout.FlatAppearance.BorderSize = 0;
            btnCheckout.Click += BtnCheckout_Click;
            panelRight.Controls.Add(btnCheckout);

            Panel panelCenter = new Panel();
            panelCenter.Dock = DockStyle.Fill;
            panelCenter.BackColor = Color.White;
            panelCenter.Padding = new Padding(20);
            this.Controls.Add(panelCenter);

            Label lblTitle = new Label();
            lblTitle.Text = "Меню пицц";
            lblTitle.Font = new Font("Arial", 26, FontStyle.Bold);
            lblTitle.Location = new Point(20, 15);
            lblTitle.AutoSize = true;
            panelCenter.Controls.Add(lblTitle);

            panelMenu.Location = new Point(20, 60);
            panelMenu.Size = new Size(0, 0);
            panelMenu.Dock = DockStyle.Fill;
            panelMenu.AutoScroll = true;
            panelMenu.FlowDirection = FlowDirection.TopDown;
            panelMenu.WrapContents = false;
            panelMenu.Padding = new Padding(10);
            panelCenter.Controls.Add(panelMenu);

            itemMargarita = new PizzaMenuItem(
                "Маргарита", 450, "marg.png",
                "Классическое", "Томатный",
                2, 0, 0, 0,
                false, false, true, true, false, false);

            itemPepperoni = new PizzaMenuItem(
                "Пепперони", 550, "pep.png",
                "Тонкое", "Томатный",
                1, 0, 3, 0,
                false, false, false, false, false, false);

            itemCountry = new PizzaMenuItem(
                "Кантри", 500, "kant.png",
                "Пышное", "Сливочный",
                1, 1, 0, 0,
                true, true, false, false, false, false);

            itemHawaiian = new PizzaMenuItem(
                "Гавайская", 520, "marg.png",
                "Классическое", "Томатный",
                2, 0, 0, 0,
                false, false, false, false, true, true);

            itemChicken = new PizzaMenuItem(
                "Куриная", 530, "pep.png",
                "Тонкое", "Барбекю",
                1, 1, 0, 0,
                true, true, false, false, false, false);

            panelMenu.Controls.Add(itemMargarita.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemPepperoni.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemCountry.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemHawaiian.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemChicken.CreatePanel(BtnAdd_Click));

            this.Resize += Form1_Resize;
            Form1_Resize(null, null);
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            int panelHeight = this.ClientSize.Height - 150;
            listBoxHistory.Size = new Size(290, panelHeight - 50);
            panelCurrentOrder.Size = new Size(370, panelHeight - 150);

            lblTotalCount.Location = new Point(20, this.ClientSize.Height - 180);
            lblTotalSum.Location = new Point(20, this.ClientSize.Height - 130);
            btnCheckout.Location = new Point(20, this.ClientSize.Height - 80);
        }

        private void BtnAdd_Click(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            Panel panel = (Panel)btn.Parent;
            PizzaMenuItem item = null;

            if (panel == itemMargarita.Panel) item = itemMargarita;
            else if (panel == itemPepperoni.Panel) item = itemPepperoni;
            else if (panel == itemCountry.Panel) item = itemCountry;
            else if (panel == itemHawaiian.Panel) item = itemHawaiian;
            else if (panel == itemChicken.Panel) item = itemChicken;

            if (item != null)
            {
                Pizza pizza = item.CreatePizza();
                currentOrder.AddPizza(pizza);

                string historyEntry = $"{DateTime.Now.ToShortTimeString()} - {pizza.Name} ({pizza.GetTotalPrice()} руб.)";
                listBoxHistory.Items.Insert(0, historyEntry);

                UpdateOrderDisplay();

                item.NumMozzarella.Value = 0;
                item.NumCheddar.Value = 0;
                item.NumPepperoni.Value = 0;
                item.NumJalapeno.Value = 0;
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
            panel.Size = new Size(350, 100);
            panel.BackColor = Color.White;
            panel.BorderStyle = BorderStyle.FixedSingle;
            panel.Margin = new Padding(5);

            Label lblName = new Label();
            lblName.Text = orderItem.Pizza.Name;
            lblName.Font = new Font("Arial", 12, FontStyle.Bold);
            lblName.Location = new Point(10, 10);
            lblName.AutoSize = true;
            panel.Controls.Add(lblName);

            orderItem.lblQuantity = new Label();
            orderItem.lblQuantity.Text = $"x{orderItem.Quantity}";
            orderItem.lblQuantity.Font = new Font("Arial", 14, FontStyle.Bold);
            orderItem.lblQuantity.ForeColor = Color.Green;
            orderItem.lblQuantity.Location = new Point(310, 10);
            orderItem.lblQuantity.AutoSize = true;
            panel.Controls.Add(orderItem.lblQuantity);

            Label lblToppings = new Label();
            lblToppings.Text = orderItem.Pizza.GetToppingsString();
            lblToppings.Font = new Font("Arial", 9, FontStyle.Regular);
            lblToppings.ForeColor = Color.Gray;
            lblToppings.Location = new Point(10, 38);
            lblToppings.AutoSize = true;
            panel.Controls.Add(lblToppings);

            Label lblPrice = new Label();
            lblPrice.Text = $"{orderItem.GetTotalPrice()} руб.";
            lblPrice.Font = new Font("Arial", 11, FontStyle.Bold);
            lblPrice.Location = new Point(10, 65);
            lblPrice.AutoSize = true;
            panel.Controls.Add(lblPrice);

            Button btnRemove = new Button();
            btnRemove.Text = "✕";
            btnRemove.Size = new Size(35, 35);
            btnRemove.Location = new Point(305, 55);
            btnRemove.Font = new Font("Arial", 14, FontStyle.Bold);
            btnRemove.BackColor = Color.Red;
            btnRemove.ForeColor = Color.White;
            btnRemove.FlatStyle = FlatStyle.Flat;
            btnRemove.FlatAppearance.BorderSize = 0;
            btnRemove.Click += (s, e) => RemoveItem_Click(orderItem);
            panel.Controls.Add(btnRemove);

            orderItem.Panel = panel;
            return panel;
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

            string orderText = $"Заказ оформлен!\nВсего пицц: {currentOrder.GetTotalPizzasCount()}\n";
            orderText += $"Сумма: {currentOrder.GetTotalSum()} руб.\n\n";

            foreach (OrderItem item in currentOrder.Items)
            {
                orderText += $"{item.Quantity}x {item.Pizza.Name} - {item.GetTotalPrice()} руб.\n";
            }

            MessageBox.Show(orderText, "Заказ успешно оформлен!", MessageBoxButtons.OK, MessageBoxIcon.Information);

            currentOrder = new Order();
            listBoxHistory.Items.Clear();
            UpdateOrderDisplay();
        }
    }
}