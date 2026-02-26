namespace WITH_pattern
{
    public class PizzaDirector
    {
        public void MakeMargarita(IPizzaBuilder builder)
        {
            builder.Reset();
            builder.SetName("Маргарита")
                   .SetBasePrice(450)
                   .SetDough(ToppingCatalog.GetDoughByName("Классическое"))
                   .SetSauce(ToppingCatalog.GetSauceByName("Томатный"))
                   .AddTopMozzarella(2)
                   .AddTopTomatoes(1)
                   .AddTopBasil(1);
        }

        public void MakePepperoni(IPizzaBuilder builder)
        {
            builder.Reset();
            builder.SetName("Пепперони")
                   .SetBasePrice(550)
                   .SetDough(ToppingCatalog.GetDoughByName("Тонкое"))
                   .SetSauce(ToppingCatalog.GetSauceByName("Томатный"))
                   .AddTopMozzarella(1)
                   .AddTopPepperoni(3);
        }

        public void MakeCountry(IPizzaBuilder builder)
        {
            builder.Reset();
            builder.SetName("Кантри")
                   .SetBasePrice(500)
                   .SetDough(ToppingCatalog.GetDoughByName("Пышное"))
                   .SetSauce(ToppingCatalog.GetSauceByName("Сливочный"))
                   .AddTopMozzarella(1)
                   .AddTopCheddar(1)
                   .AddTopChicken(1)
                   .AddTopMushrooms(1);
        }

        public void MakeHawaiian(IPizzaBuilder builder)
        {
            builder.Reset();
            builder.SetName("Гавайская")
                   .SetBasePrice(520)
                   .SetDough(ToppingCatalog.GetDoughByName("Классическое"))
                   .SetSauce(ToppingCatalog.GetSauceByName("Томатный"))
                   .AddTopMozzarella(2)
                   .AddTopHam(1)
                   .AddTopPineapple(1);
        }

        public void MakeChicken(IPizzaBuilder builder)
        {
            builder.Reset();
            builder.SetName("Куриная")
                   .SetBasePrice(530)
                   .SetDough(ToppingCatalog.GetDoughByName("Тонкое"))
                   .SetSauce(ToppingCatalog.GetSauceByName("Барбекю"))
                   .AddTopMozzarella(1)
                   .AddTopCheddar(1)
                   .AddTopChicken(1)
                   .AddTopMushrooms(1);
        }

        public void MakeCustomPizza(IPizzaBuilder builder, Dough dough, Sauce sauce)
        {
            builder.Reset();
            builder.SetName("Ваша пицца")
                   .SetBasePrice(200)
                   .SetIsCustomPizza(true)
                   .SetDough(dough)
                   .SetSauce(sauce);
        }
    }
}