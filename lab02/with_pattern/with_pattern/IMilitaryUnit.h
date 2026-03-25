#pragma once
#include <string>
#include <vector>

// ============================================================
//  Интерфейс Компоновщика
//  Реализуют: Unit (лист) и Formation (узел)
// ============================================================
class IMilitaryUnit {
public:
    virtual ~IMilitaryUnit() = default;

    // Общие методы — работают одинаково для юнита и подразделения
    virtual float       calculatePower()   const = 0;
    virtual int         getTotalCost()     const = 0;
    virtual bool        hasBonus()         const = 0;
    virtual std::string getName()          const = 0;
    virtual std::string getDescription()   const = 0;
    virtual std::string getUnitType()      const = 0;  // "INFANTRY","VEHICLE","SQUAD","PLATOON"

    // Метод различения: лист или узел
    virtual bool isComposite() const = 0;

    // Методы узла (Formation). У Unit — заглушки (return false / ничего не делают)
    virtual bool                            add(IMilitaryUnit* child)   { return false; }
    virtual void                            remove(int index)           {}
    virtual std::vector<IMilitaryUnit*>     getChildren()         const { return {}; }
};
