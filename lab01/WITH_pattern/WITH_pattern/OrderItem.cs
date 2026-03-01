using System.Windows.Forms;

namespace WITH_pattern
{
    public class OrderItem
    {
        public Pizza Pizza { get; set; }
        public int Quantity { get; set; }
        public Label lblQuantity { get; set; } // Для обратной связи с UI
        public Panel Panel { get; set; }

        public OrderItem(Pizza pizza)
        {
            this.Pizza = pizza;
            this.Quantity = 1;
        }

        public int CalculateTotal()
        {
            return Pizza.CalculateTotal() * Quantity;
        }

        public void UpdateQuantityDisplay()
        {
            if (lblQuantity != null)
                lblQuantity.Text = Quantity.ToString();
        }
    }
}