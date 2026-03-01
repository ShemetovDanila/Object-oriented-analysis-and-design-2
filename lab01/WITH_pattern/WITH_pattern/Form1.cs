using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
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

            lblTotalSum = new Label();
            lblTotalCount = new Label();
            panelMenu = new FlowLayoutPanel();
            panelCurrentOrder = new FlowLayoutPanel();
            currentOrder = new Order();
            orderHistoryList = new List<string>();
            _director = new PizzaDirector();

            InitializeComponentsManual();
        }

        private void InitializeComponentsManual()
        {
            Panel panelRight = new Panel { BackColor = Color.FromArgb(255, 245, 200), Dock = DockStyle.Right, Width = 400 };
            this.Controls.Add(panelRight);

            Label lblOrderTitle = new Label
            {
                Text = "Текущий заказ",
                Font = new Font("Arial", 18, FontStyle.Bold),
                Dock = DockStyle.Top,
                Height = 60,
                TextAlign = ContentAlignment.MiddleCenter,
                BackColor = Color.FromArgb(255, 245, 200)
            };
            panelRight.Controls.Add(lblOrderTitle);

            panelCurrentOrder.Dock = DockStyle.Fill;
            panelCurrentOrder.AutoScroll = true;
            panelCurrentOrder.FlowDirection = FlowDirection.TopDown;
            panelCurrentOrder.WrapContents = false;
            panelCurrentOrder.Padding = new Padding(15, 65, 15, 15);
            panelRight.Controls.Add(panelCurrentOrder);

            Panel panelBottomRight = new Panel { Dock = DockStyle.Bottom, Height = 230, BackColor = Color.FromArgb(255, 245, 200) };
            panelRight.Controls.Add(panelBottomRight);

            lblTotalCount.Text = "Всего пицц: 0";
            lblTotalCount.Font = new Font("Arial", 13, FontStyle.Bold);
            lblTotalCount.Location = new Point(20, 20);
            lblTotalCount.AutoSize = true;
            panelBottomRight.Controls.Add(lblTotalCount);

            lblTotalSum.Text = "Итого: 0 руб.";
            lblTotalSum.Font = new Font("Arial", 18, FontStyle.Bold);
            lblTotalSum.Location = new Point(20, 55);
            lblTotalSum.AutoSize = true;
            panelBottomRight.Controls.Add(lblTotalSum);

            btnCheckout = new Button
            {
                Text = "Оформить заказ",
                Size = new Size(360, 50),
                Location = new Point(20, 100),
                BackColor = Color.FromArgb(255, 140, 0),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Arial", 14, FontStyle.Bold)
            };
            btnCheckout.FlatAppearance.BorderSize = 0;
            btnCheckout.Click += BtnCheckout_Click;
            panelBottomRight.Controls.Add(btnCheckout);

            btnHistory = new Button
            {
                Text = "📋 История заказов",
                Size = new Size(360, 45),
                Location = new Point(20, 165),
                BackColor = Color.FromArgb(100, 100, 100),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat
            };
            btnHistory.FlatAppearance.BorderSize = 0;
            btnHistory.Click += BtnHistory_Click;
            panelBottomRight.Controls.Add(btnHistory);

            Panel panelCenter = new Panel { Dock = DockStyle.Fill, BackColor = Color.White };
            this.Controls.Add(panelCenter);

            Label lblTitle = new Label { Text = "Меню пицц", Font = new Font("Arial", 28, FontStyle.Bold), Location = new Point(40, 30), AutoSize = true };
            panelCenter.Controls.Add(lblTitle);

            panelMenu.Location = new Point(40, 100);
            panelMenu.Size = new Size(1100, 700);
            panelMenu.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Bottom;
            panelMenu.AutoScroll = true;
            panelMenu.FlowDirection = FlowDirection.TopDown;
            panelMenu.WrapContents = false;
            panelCenter.Controls.Add(panelMenu);

            PizzaBuilder b = new PizzaBuilder();
            _director.MakeCustomPizza(b); itemCustom = new PizzaMenuItem("🍕 Конструктор", 200, "myPizza.png", b.GetResult(), true);
            _director.MakeMargarita(b); itemMargarita = new PizzaMenuItem("Маргарита", 450, "marg.png", b.GetResult(), false);
            _director.MakePepperoni(b); itemPepperoni = new PizzaMenuItem("Пепперони", 550, "pep.png", b.GetResult(), false);
            _director.MakeCountry(b); itemCountry = new PizzaMenuItem("Кантри", 500, "kant.png", b.GetResult(), false);
            _director.MakeHawaiian(b); itemHawaiian = new PizzaMenuItem("Гавайская", 520, "pineapple.png", b.GetResult(), false);
            _director.MakeChicken(b); itemChicken = new PizzaMenuItem("Куриная", 530, "chicken.png", b.GetResult(), false);

            panelMenu.Controls.Add(itemCustom.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemMargarita.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemPepperoni.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemCountry.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemHawaiian.CreatePanel(BtnAdd_Click));
            panelMenu.Controls.Add(itemChicken.CreatePanel(BtnAdd_Click));
        }

        private void BtnAdd_Click(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            Panel p = (Panel)btn.Parent;
            PizzaMenuItem selected = null;

            if (p == itemCustom.Panel) selected = itemCustom;
            else if (p == itemMargarita.Panel) selected = itemMargarita;
            else if (p == itemPepperoni.Panel) selected = itemPepperoni;
            else if (p == itemCountry.Panel) selected = itemCountry;
            else if (p == itemHawaiian.Panel) selected = itemHawaiian;
            else if (p == itemChicken.Panel) selected = itemChicken;

            if (selected != null)
            {
                currentOrder.AddPizza(selected.CreatePizzaWithToppings());
                UpdateOrderDisplay();
                foreach (var ctrl in selected.ToppingControls) ctrl.Value.Value = 0;
            }
        }

        private void UpdateOrderDisplay()
        {
            panelCurrentOrder.Controls.Clear();
            foreach (var orderItem in currentOrder.Items)
            {
                panelCurrentOrder.Controls.Add(CreateOrderItemPanel(orderItem));
            }
            lblTotalSum.Text = $"Итого: {currentOrder.GetTotalSum()} руб.";
            lblTotalCount.Text = $"Всего пицц: {currentOrder.GetTotalPizzasCount()}";
        }

        private Panel CreateOrderItemPanel(OrderItem orderItem)
        {
            Panel p = new Panel { Size = new Size(350, 120), BackColor = Color.White, BorderStyle = BorderStyle.FixedSingle, Margin = new Padding(0, 0, 0, 10) };

            Label name = new Label { Text = orderItem.Pizza.name, Font = new Font("Arial", 11, FontStyle.Bold), Location = new Point(10, 10), AutoSize = true };
            p.Controls.Add(name);

            Panel qtyContainer = new Panel { Size = new Size(100, 30), Location = new Point(235, 10), BorderStyle = BorderStyle.FixedSingle };
            Button btnMinus = new Button { Text = "−", Size = new Size(30, 30), Location = new Point(0, -1), FlatStyle = FlatStyle.Flat };
            btnMinus.FlatAppearance.BorderSize = 0;
            btnMinus.Click += (s, e) =>
            {
                if (orderItem.Quantity > 1) { orderItem.Quantity--; UpdateOrderDisplay(); }
                else { currentOrder.RemoveItem(orderItem); UpdateOrderDisplay(); }
            };

            Button btnPlus = new Button { Text = "+", Size = new Size(30, 30), Location = new Point(68, -1), FlatStyle = FlatStyle.Flat };
            btnPlus.FlatAppearance.BorderSize = 0;
            btnPlus.Click += (s, e) => { orderItem.Quantity++; UpdateOrderDisplay(); };

            Label qty = new Label { Text = orderItem.Quantity.ToString(), Size = new Size(38, 28), Location = new Point(31, 1), TextAlign = ContentAlignment.MiddleCenter, Font = new Font("Arial", 10, FontStyle.Bold) };

            qtyContainer.Controls.Add(btnMinus);
            qtyContainer.Controls.Add(btnPlus);
            qtyContainer.Controls.Add(qty);
            p.Controls.Add(qtyContainer);

            Label details = new Label { Text = $"{orderItem.Pizza.Dough.name} | {orderItem.Pizza.Sauce.name}", Location = new Point(10, 35), ForeColor = Color.Gray, AutoSize = true };
            Label toppings = new Label { Text = orderItem.Pizza.GetToppingsString(), Location = new Point(10, 55), Size = new Size(330, 35), ForeColor = Color.DimGray, Font = new Font("Arial", 7) };
            Label price = new Label { Text = $"{orderItem.GetTotalPrice()} руб.", Font = new Font("Arial", 10, FontStyle.Bold), Location = new Point(10, 95), AutoSize = true };

            p.Controls.Add(details); p.Controls.Add(toppings); p.Controls.Add(price);
            return p;
        }

        private void BtnCheckout_Click(object sender, EventArgs e)
        {
            if (currentOrder.Items.Count == 0) return;
            string entry = $"{DateTime.Now.ToShortTimeString()} - Заказ на {currentOrder.GetTotalSum()} руб.";
            orderHistoryList.Add(entry);
            if (historyForm != null) historyForm.AddOrderToHistory(entry);
            MessageBox.Show("Заказ оформлен!");
            currentOrder = new Order();
            UpdateOrderDisplay();
        }

        private void BtnHistory_Click(object sender, EventArgs e)
        {
            if (historyForm == null || historyForm.IsDisposed) historyForm = new Form2(this);
            historyForm.Show();
        }
    }
}