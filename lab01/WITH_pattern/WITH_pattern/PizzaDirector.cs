using System.Collections.Generic;

namespace WITH_pattern
{
    public class PizzaDirector
    {
        private IPizzaBuilder _builder;

        public void SetBuilder(IPizzaBuilder builder)
        {
            _builder = builder;
        }

        public Pizza MakeMargarita(Dough dough, Sauce sauce, Topping mozzarella, Topping tomatoes, Topping basil)
        {
            _builder.Reset();
            return _builder.SetName("Маргарита")
                          .SetDough(dough)
                          .SetSauce(sauce)
                          .SetBasePrice(450)
                          .SetIsCustomPizza(false)
                          .AddTopping(mozzarella, 2, isBase: true)
                          .AddTopping(tomatoes, 1, isBase: true)
                          .AddTopping(basil, 1, isBase: true)
                          .Build();
        }

        public Pizza MakePepperoni(Dough dough, Sauce sauce, Topping mozzarella, Topping pepperoni)
        {
            _builder.Reset();
            return _builder.SetName("Пепперони")
                          .SetDough(dough)
                          .SetSauce(sauce)
                          .SetBasePrice(550)
                          .SetIsCustomPizza(false)
                          .AddTopping(mozzarella, 1, isBase: true)
                          .AddTopping(pepperoni, 3, isBase: true)
                          .Build();
        }

        public Pizza MakeCountry(Dough dough, Sauce sauce, Topping mozzarella, Topping cheddar, Topping chicken, Topping mushrooms)
        {
            _builder.Reset();
            return _builder.SetName("Кантри")
                          .SetDough(dough)
                          .SetSauce(sauce)
                          .SetBasePrice(500)
                          .SetIsCustomPizza(false)
                          .AddTopping(mozzarella, 1, isBase: true)
                          .AddTopping(cheddar, 1, isBase: true)
                          .AddTopping(chicken, 1, isBase: true)
                          .AddTopping(mushrooms, 1, isBase: true)
                          .Build();
        }

        public Pizza MakeHawaiian(Dough dough, Sauce sauce, Topping mozzarella, Topping ham, Topping pineapple)
        {
            _builder.Reset();
            return _builder.SetName("Гавайская")
                          .SetDough(dough)
                          .SetSauce(sauce)
                          .SetBasePrice(520)
                          .SetIsCustomPizza(false)
                          .AddTopping(mozzarella, 2, isBase: true)
                          .AddTopping(ham, 1, isBase: true)
                          .AddTopping(pineapple, 1, isBase: true)
                          .Build();
        }

        public Pizza MakeChicken(Dough dough, Sauce sauce, Topping mozzarella, Topping cheddar, Topping chicken, Topping mushrooms)
        {
            _builder.Reset();
            return _builder.SetName("Куриная")
                          .SetDough(dough)
                          .SetSauce(sauce)
                          .SetBasePrice(530)
                          .SetIsCustomPizza(false)
                          .AddTopping(mozzarella, 1, isBase: true)
                          .AddTopping(cheddar, 1, isBase: true)
                          .AddTopping(chicken, 1, isBase: true)
                          .AddTopping(mushrooms, 1, isBase: true)
                          .Build();
        }

        public Pizza MakeCustomPizza(Dough dough, Sauce sauce)
        {
            _builder.Reset();
            return _builder.SetName("Ваша пицца")
                          .SetDough(dough)
                          .SetSauce(sauce)
                          .SetBasePrice(200)
                          .SetIsCustomPizza(true)
                          .Build();
        }
    }
}