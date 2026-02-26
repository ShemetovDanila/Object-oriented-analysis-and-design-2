namespace WITH_pattern
{
    public interface IPizzaBuilder
    {
        void Reset();
        IPizzaBuilder SetName(string name);
        IPizzaBuilder SetDough(Dough dough);
        IPizzaBuilder SetSauce(Sauce sauce);
        IPizzaBuilder SetBasePrice(int price);
        IPizzaBuilder SetIsCustomPizza(bool isCustom);
        IPizzaBuilder AddTopMozzarella(int count);
        IPizzaBuilder AddTopCheddar(int count);
        IPizzaBuilder AddTopParmesan(int count);
        IPizzaBuilder AddTopDorBlue(int count);
        IPizzaBuilder AddTopPepperoni(int count);
        IPizzaBuilder AddTopHam(int count);
        IPizzaBuilder AddTopChicken(int count);
        IPizzaBuilder AddTopBacon(int count);
        IPizzaBuilder AddTopMushrooms(int count);
        IPizzaBuilder AddTopTomatoes(int count);
        IPizzaBuilder AddTopOlives(int count);
        IPizzaBuilder AddTopJalapeno(int count);
        IPizzaBuilder AddTopBellPepper(int count);
        IPizzaBuilder AddTopOnion(int count);
        IPizzaBuilder AddTopPineapple(int count);
        IPizzaBuilder AddTopBasil(int count);
        IPizzaBuilder AddTopArugula(int count);
        Pizza GetResult();
    }
}