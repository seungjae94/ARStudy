#include "CodeBaseCreater.h"

int main()
{
    //CodeBaseCreater::RegisterUsers({ "JJW", "PJY", "PKT", "KKY", "LSJ", "USM", "KSW", "PTH", "BYD" });
    CodeBaseCreater::RegisterUsers({ "JJW", "LSJ" });

    CodeBaseCreater::Create("01_Queue", "Queue", EAnswerType::Int);
    CodeBaseCreater::Create("01_Queue", "Queue2", EAnswerType::Int);
    CodeBaseCreater::Create("01_Queue", "Card2", EAnswerType::Int);
}