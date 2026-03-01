namespace WITH_pattern
{
    public class PizzaDirector
    {
        public void MakeMargarita(IPizzaBuilder builder) => builder.Reset().SetName("Маргарита").SetBasePrice(450).SetDough("Классическое").SetSauce("Томатный").AddTopping("Моцарелла").AddTopping("Базилик");
        public void MakePepperoni(IPizzaBuilder builder) => builder.Reset().SetName("Пепперони").SetBasePrice(550).SetDough("Классическое").SetSauce("Томатный").AddTopping("Пепперони").AddTopping("Моцарелла");
        public void MakeCountry(IPizzaBuilder builder) => builder.Reset().SetName("Кантри").SetBasePrice(500).SetDough("Толстое").SetSauce("Барбекю").AddTopping("Бекон").AddTopping("Лук");
        public void MakeHawaiian(IPizzaBuilder builder) => builder.Reset().SetName("Гавайская").SetBasePrice(520).SetDough("Классическое").SetSauce("Томатный").AddTopping("Курица").AddTopping("Ананас");
        public void MakeChicken(IPizzaBuilder builder) => builder.Reset().SetName("Куриная").SetBasePrice(530).SetDough("Тонкое").SetSauce("Сливочный").AddTopping("Курица").AddTopping("Грибы");
        public void MakeCustomPizza(IPizzaBuilder builder) => builder.Reset().SetName("Ваша пицца").SetBasePrice(200).SetIsCustom(true).SetDough("Классическое").SetSauce("Томатный");
    }
}