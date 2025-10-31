#include "Player.hpp"
#include "Game.hpp"
#include "Abilities.hpp"
#include <iostream>
#include <limits>

Player::Player(std::string name,
        int authority,
        Deck deck)
    : name_(name),
      authority_(authority),
      deck_(deck) {}

Player::~Player() = default;

std::vector<Champion*> Player::getGuards() const {
    std::vector<Champion*> guards = {};
    for (Champion* champion : champions_){
        if (champion->isGuard()){
            guards.push_back(champion);
        }
    }
    return guards;
}

void Player::removeChampion(Champion* champion){
    for (std::vector<Champion*>::iterator it = champions_.begin(); it != champions_.end();)
    {
        // *it sert à récupérer l'objet Card pointé par l'itérateur
        if (champion->id() == (*it)->id()){
            it = champions_.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

void Player::draw(int n){
    int cardsLeft = deck_.getDeckContents().size();
    if(n >= cardsLeft){
        // Je tire toutes les cartes restantes dans le deck
        std::vector<Card*> listLeft = deck_.draw(cardsLeft);
        hand_.insert(hand_.end(), listLeft.cbegin(), listLeft.cend());
        // Je remélange la défausse dans le deck et je continue à piocher
        deck_.setDeck(discardPile_);
        deck_.shuffle();
        // Je vide la défausse
        discardPile_.clear();
        // Je pioche le reste des cartes
        std::vector<Card*> listNew = deck_.draw(n-cardsLeft);
        hand_.insert(hand_.end(), listNew.cbegin(), listNew.cend());
    } else {
        // Je pioche normalement
        std::vector<Card*> drawn = deck_.draw(n);
        hand_.insert(hand_.end(), drawn.cbegin(), drawn.cend());
    }
}

void Player::play(Card* card){
    bool found = false;
    for (std::vector<Card*>::iterator it = hand_.begin(); it != hand_.end();)
    {
        if (card->id() == (*it)->id()){
            it = hand_.erase(it);
            found = true;
            break;
        }
        else {
            ++it;
        }
    }
    // Ne devrait pas arriver car idéalement on ne sera proposé que des cartes en main, mais par sécurité on vérifie quand même
    if(!found){
        std::cout << "Card not found in hand, cannot play.\n";
        return;
    }

    if(card->isChampion()){
        champions_.push_back((Champion*)card);
    } else {
        inPlay_.push_back(card);
    }
}

bool Player::buy(Card* card, Market market){
    if(gold_ >= card->cost()){
        gold_ -= card->cost();
        if(nextActionBuyOnDeck && card->type() == CardType::Action){
            deck_.addToTop(card);
            nextActionBuyOnDeck = false;
        } else if (nextBuyInHand){
            hand_.push_back(card);
            nextBuyInHand = false;
        } else if (nextBuyTopOfDeck){
            deck_.addToTop(card);
        } else {
            discardPile_.push_back(card);
        }
        if(!(card->name() == "Fire Gem")){
            market.sell(card);
        }
        return true;
    } else {
        std::cout << "Pas assez d'or pour acheter cette carte : elle coûte " << card->cost() << " or et vous en avez " << gold_ << "\n";
        return false;
        // Out : pas assez de golds, il faut modifier le player.hpp j'ai mis 15 golds de départ pour que ça marche
    }  
}

bool Player::godmodeBuy(Card* card, Market market){
    if(gold_ >= card->cost()){
        gold_ -= card->cost();
        hand_.push_back(card);
        if(!(card->name() == "Fire Gem")){
            market.godmodeSell(card);
        }
        return true;
    } else {
        std::cout << "Pas assez d'or pour acheter cette carte : elle coûte " << card->cost() << " or et vous en avez " << gold_ << "\n";
        return false;
        // Out : pas assez de golds, il faut modifier le player.hpp j'ai mis 15 golds de départ pour que ça marche
    }  
}

void Player::discard(int amount){
    std::cout << name_ << ", you have to discard " << amount << " cards :\n";
    for (int i=0;i<amount;i++){
        if ((int)hand_.size() > 0){

            for (int i=0;i<(int)hand_.size();i++){
                std::cout << " - " << i+1 << "\n";
                hand_[i]->printCardInfo();
            }

            int discardChoice;
            while(!(std::cin >> discardChoice) || discardChoice < 1 || discardChoice > (int)hand_.size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            // Je rajoute la carte dans la pile de discard avant juste pour être sur de pas avoir de problème en supprimant la carte 
            discardPile_.push_back(hand_[discardChoice-1]);
            std::cout << name_ << " has discarded the following card :\n";
            hand_[discardChoice-1]->printCardInfo();

            // Supprime la carte
            for (std::vector<Card*>::iterator it = hand_.begin(); it != hand_.end();)
            {
                // *it sert à récupérer l'objet Card pointé par l'itérateur
                if (hand_[discardChoice-1]->id() == (*it)->id()){
                    it = hand_.erase(it);
                    break;
                } else {
                    ++it;
                }
            }
        } else {
            std::cout << "No cards left in hand : cannot discard anymore";
            break;
        } 
    }
}

bool Player::cardEffectSacrifice(int amount){
    std::cout << "You can sacrifice " << amount << " cards :\n";
    for (int i=0;i<amount;i++){
        std::cout << "What do you want to do ? 1. Sacrifice from hand - 2. Sacrifice from discard pile - 3. Do nothing\n";

        int choice;
            while(!(std::cin >> choice) || choice < 1 || choice > 3){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (choice == 3){
            return false;
        }

        switch(choice){
            case 1:
                if ((int)hand_.size() > 0){
                    for (int i=0;i<(int)hand_.size();i++){
                        std::cout << " - " << i+1 << "\n";
                        hand_[i]->printCardInfo();
                    }

                    int handSacrificeChoice;
                    while(!(std::cin >> handSacrificeChoice) || handSacrificeChoice < 1 || handSacrificeChoice > (int)hand_.size()){
                        std::cout << "Invalid input. Please enter a valid choice: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }

                    // Je rajoute la carte dans la pile de discard avant juste pour être sur de pas avoir de problème en supprimant la carte
                    Game::sacrifice(hand_[handSacrificeChoice-1]); 
                    std::cout << name_ << " has sacrificed the following card :\n";
                    hand_[handSacrificeChoice-1]->printCardInfo();

                    // Supprime la carte
                    for (std::vector<Card*>::iterator it = hand_.begin(); it != hand_.end();)
                    {
                        // *it sert à récupérer l'objet Card pointé par l'itérateur
                        if (hand_[handSacrificeChoice-1]->id() == (*it)->id()){
                            it = hand_.erase(it);
                            break;
                        } else {
                            ++it;
                        }
                    }
                    return true;
                } else {
                    std::cout << "No cards left in hand : cannot sacrifice";
                    return false;
                }
                break;
            case 2:
                if ((int)discardPile_.size() > 0){
                    for (int i=0;i<(int)discardPile_.size();i++){
                        std::cout << " - " << i+1 << "\n";
                        discardPile_[i]->printCardInfo();
                    }

                    int discardSacrificeChoice;
                    while(!(std::cin >> discardSacrificeChoice) || discardSacrificeChoice < 1 || discardSacrificeChoice > (int)hand_.size()){
                        std::cout << "Invalid input. Please enter a valid choice: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }

                    // Je rajoute la carte dans la pile de discard avant juste pour être sur de pas avoir de problème en supprimant la carte 
                    Game::sacrifice(discardPile_[discardSacrificeChoice-1]);
                    std::cout << name_ << " has sacrificed the following card :\n";
                    discardPile_[discardSacrificeChoice-1]->printCardInfo();

                    // Supprime la carte
                    for (std::vector<Card*>::iterator it = discardPile_.begin(); it != discardPile_.end();)
                    {
                        // *it sert à récupérer l'objet Card pointé par l'itérateur
                        if (discardPile_[discardSacrificeChoice-1]->id() == (*it)->id()){
                            it = discardPile_.erase(it);
                            break;
                        } else {
                            ++it;
                        }
                    }
                    return true;
                } else {
                    std::cout << "No cards left in hand : cannot sacrifice";
                    return false;
                }
                break;
            default: return false;
        }
    }
    return false;
}

void Player::prepareFriendlyChampion(){
    std::vector<Champion*> unprepared = {};
    for (Champion* champion : champions_){
        for (const auto& ab : champion->abilities()){
            if(Card::triggerToString(ab.trigger) == "Expend" || Card::triggerToString(ab.trigger) == "ExpendChoice" ){
                if(ab.used){
                    unprepared.push_back(champion);
                    break;
                }
            }
        }
    }
    if (unprepared.empty()){
        std::cout << "You do not have a champion you can prepare\n";
    } else {
        std::cout << "Which champion do you want to prepare ? :\n";
        for (int i=0;i<(int)unprepared.size();i++){
            std::cout << " - " << i+1 << "\n";
            unprepared[i]->printCardInfo();
        }

        int prepareChoice;
        while(!(std::cin >> prepareChoice) || prepareChoice < 1 || prepareChoice > (int)unprepared.size()){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        for (Champion* champion : champions_){
            if(champion->id() == unprepared[prepareChoice-1]->id()){
                for (auto& ab : champion->abilities()){
                    if(Card::triggerToString(ab.trigger) == "Expend" || Card::triggerToString(ab.trigger) == "ExpendChoice" ){
                        if(ab.used){
                            ab.used = false;
                        }
                    }
                }
            }
        }
    }
}

void Player::getChampionFromDiscardToDeck(){
    std::vector<Champion*> championsDiscard = {};
    for (Card* card : discardPile_){
        if(card->isChampion()){
            championsDiscard.push_back((Champion*)card);
        }
    }
    if(championsDiscard.size() == 0){
        std::cout << "There are no champions in the discard pile\n";
    } else {
        std::cout << "Which champion do you want to bring to the top of your deck ?\n";
        for (int i=0;i<(int)championsDiscard.size();i++){
            std::cout << " - " << i+1 << "\n";
            championsDiscard[i]->printCardInfo();
        }
        int choice;
        while(!(std::cin >> choice) || choice < 1 || choice > (int)championsDiscard.size()){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        deck_.addToTop(championsDiscard[choice-1]);
        for (std::vector<Card*>::iterator it = discardPile_.begin(); it != discardPile_.end();)
        {
            // *it sert à récupérer l'objet Card pointé par l'itérateur
            if (championsDiscard[choice-1]->id() == (*it)->id()){
                it = discardPile_.erase(it);
                break;
            } else {
                    ++it;
            }
        }
        std::cout << "Champion added to the top of the deck :\n";
        championsDiscard[choice-1]->printCardInfo();
    }
}

void Player::getCardFromDiscardToDeck(){
    std::cout << "Do you want to put a card from your discard pile on top of your deck ? (1. Yes /2. No)\n";
    int drawChoice;
    while(!(std::cin >> drawChoice) || drawChoice < 1 || drawChoice > 2){
        std::cout << "Invalid input. Please enter a valid choice: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    if(drawChoice == 1){
        if(discardPile_.size() == 0){
            std::cout << "There are no champions in the discard pile\n";
        } else {
            std::cout << "Which card do you want to bring to the top of your deck ?\n";
            for (int i=0;i<(int)discardPile_.size();i++){
                std::cout << " - " << i+1 << "\n";
                discardPile_[i]->printCardInfo();
            }
            int choice;
            while(!(std::cin >> choice) || choice < 1 || choice > (int)discardPile_.size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            deck_.addToTop(discardPile_[choice-1]);
            for (std::vector<Card*>::iterator it = discardPile_.begin(); it != discardPile_.end();)
            {
                // *it sert à récupérer l'objet Card pointé par l'itérateur
                if (discardPile_[choice-1]->id() == (*it)->id()){
                    it = discardPile_.erase(it);
                    break;
                } else {
                        ++it;
                }
            }
            std::cout << "Champion added to the top of the deck :\n";
            discardPile_[choice-1]->printCardInfo();
        }
    } else if (drawChoice == 2){
        std::cout << "No card taken from your discard pile.\n";
    } 
}

bool Player::isFactionInPlay(Faction faction){
    for (Card* card : inPlay_){
        if(card->faction() == faction){
            return true;
        }
    }
    for (Champion* champion : champions_){
        if(champion->faction() == faction){
            return true;
        }
    }
    return false;
}

bool Player::isFactionInPlayExclude(Faction faction, Card* card){
    for (Card* c : inPlay_){
        if(c->faction() == faction && card->id() != c->id()){
            return true;
        }
    }
    for (Champion* champion : champions_){
        if(champion->faction() == faction && card->id() != champion->id()){
            return true;
        }
    }
    return false;
}

bool Player::isGuarded(){
    for (Champion* champion : champions_){
        if (champion->isGuard()){
            return true;
        }
    }
    return false;
}

void Player::stunChampion(){
    if(champions_.size() == 0){
        std::cout << "This player doesnt have a stunnable champion";
    } else {
        if(isGuarded()){
            std::vector<Champion*> guards = getGuards();
            std::cout << "This opponent is guarded, you can only stun his guards. Which guard do you want to stun ? :\n";
            for (int i=0;i<(int)guards.size();i++){
                std::cout << " - " << i+1 << "\n";
                guards[i]->printCardInfo();
            }

            int guardChoice;
            while(!(std::cin >> guardChoice) || guardChoice < 1 || guardChoice > (int)guards.size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            std::cout << "The following champion was stunned :\n";
            guards[guardChoice-1]->printCardInfo();

            discardPile_.push_back(guards[guardChoice-1]);

            // On enlève le garde stun
            for (std::vector<Champion*>::iterator it = champions_.begin(); it != champions_.end();)
            {
                // *it sert à récupérer l'objet Card pointé par l'itérateur
                if (guards[guardChoice-1]->id() == (*it)->id()){
                    it = champions_.erase(it);
                    break;
                } else {
                        ++it;
                }
            }
        } else {
            std::cout << "Which champion do you want to stun ? :\n";
            for (int i=0;i<(int)champions_.size();i++){
                std::cout << " - " << i+1 << "\n";
                champions_[i]->printCardInfo();
            }

            int championChoice;
            while(!(std::cin >> championChoice) || championChoice < 1 || championChoice > (int)champions_.size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            std::cout << "The following champion was stunned :\n";
            champions_[championChoice-1]->printCardInfo();

            discardPile_.push_back(champions_[championChoice-1]);

            for (std::vector<Champion*>::iterator it = champions_.begin(); it != champions_.end();)
            {
                // *it sert à récupérer l'objet Card pointé par l'itérateur
                if (champions_[championChoice-1]->id() == (*it)->id()){
                    it = champions_.erase(it);
                    break;
                } else {
                        ++it;
                }
            }
        }
    }
}

void Player::attack(Player* player){
    if(player->isGuarded()){
        std::vector<Champion*> guards = player->getGuards();
        std::cout << player->getName() << " is guarded, you can only attack his guards !";
        std::cout << "Which guard do you want to attack ? : ";
        for (int i=0;i<(int)guards.size();i++){
            std::cout << " - " << i+1 << ". " << guards[i]->name();
        }
        std::cout << "\n";
        int guardChoice;
        while(!(std::cin >> guardChoice) || guardChoice < 1 || guardChoice > (int)guards.size()){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if(guards[guardChoice-1]->takeDamage(combat_)){
            combat_ = combat_ - guards[guardChoice-1]->getMaxShield();
            player->removeChampion(guards[guardChoice-1]);
        } else {
            std::cout << "Pas assez de combat pour tuer ce garde\n";
        }
    } else {
        std::cout << "What do you want to attack ? :\n1. " << player->getName() << " - 2. Their champions";
        int choice;
        while(!(std::cin >> choice) || choice < 1 || choice > 2){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        if (choice == 1){
            int temp = combat_-player->getAuthority();
            player->setAuthority(player->getAuthority()-combat_);
            // Pour ne pas perdre du combat lors d'un overkill
            if (temp > 0){
                combat_ = temp;
            } else {
                combat_ = 0;
            }
        } else if (choice == 2){
            for (int i=0;i<(int)player->getChampions().size();i++){
            std::cout << " - " << i+1 << ". " << player->getChampions()[i]->name();
            }
            std::cout << "\n";

            int championChoice;
            while(!(std::cin >> championChoice) || championChoice < 1 || championChoice > (int)champions_.size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            if(player->getChampions()[championChoice-1]->takeDamage(combat_)){
                combat_ = combat_ - player->getChampions()[championChoice-1]->getMaxShield();
                player->removeChampion(player->getChampions()[championChoice-1]);
            } else {
                std::cout << "Pas assez de combat pour tuer ce champion\n";
            }
        }
    }
}

void Player::useAbility(int cardChoice){
    if(cardChoice <= (int)champions_.size()){

        Champion* chosenCard = champions_[cardChoice-1];
                            
        std::cout << "The card you picked has the following abilities :\n";
        for(auto& ab : chosenCard->abilities()){
            ab.printAbility();
        }
        std::cout << "Which type of ability do you want to use ? (all the abilities with the same trigger will be activated at once.):\n";
        std::cout << "1. Ally - 2. Expend - 3. ExpendChoice - 4. Sacrifice - 5. Return.\n";

        int typeChoice;
        while(!(std::cin >> typeChoice) || typeChoice < 1 || typeChoice > 5){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch(typeChoice){
            case 1:
                if(chosenCard->hasAbilitiesWithTrigger(Trigger::Ally)){
                    if(isFactionInPlayExclude(chosenCard->faction(),chosenCard)){
                        for(auto& ab : chosenCard->abilities()){
                            if (ab.trigger == Trigger::Ally){
                                if(!ab.used){
                                    Game::smartAbilityExecute(this,ab);
                                } else {
                                    std::cout << "This ability has already been used !\n";
                                    break;
                                }
                            }
                        }
                    } else {
                        std::cout << "This card cannot activate its ally ability, there are no other cards of the same faction on your board.\n";
                    }
                } else {
                    std::cout << "This card does not have an ally ability.\n";
                }
                break;
            case 2:
                if(chosenCard->hasAbilitiesWithTrigger(Trigger::Expend)){
                    for(auto& ab : chosenCard->abilities()){
                        if (ab.trigger == Trigger::Expend){
                            if(!ab.used){
                                Game::smartAbilityExecute(this,ab);
                            } else {
                                std::cout << "This ability has already been used !\n";
                                break;
                            }
                        } 
                    }
                } else {
                    std::cout << "This card does not have an expend ability.\n";
                }
                break;
            case 3:
                if(chosenCard->hasAbilitiesWithTrigger(Trigger::ExpendChoice)){
                    std::vector<Card::CardAbility*> choiceAbilities = {};
                    for(auto& ab : chosenCard->abilities()){
                        if (ab.trigger == Trigger::ExpendChoice){
                            if(!ab.used){
                                choiceAbilities.push_back(&ab);
                            } else {
                                std::cout << "This ability has already been used !\n";
                                break;
                            }
                        } 
                    }
                    std::cout << "Chose the effect you want to use :\n";
                    for (int i=0;i<(int)choiceAbilities.size();i++){
                        std::cout << " - " << i+1 << "\n";
                        choiceAbilities[i]->printAbility();
                        // On met toutes les capacités à used pour ne pas pouvoir utiliser chaque choix séparément.
                        choiceAbilities[i]->used = true;
                    }
                    int effectChoice;
                    while(!(std::cin >> effectChoice) || effectChoice < 1 || effectChoice > (int)choiceAbilities.size()){
                        std::cout << "Invalid input. Please enter a valid choice: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    Game::smartAbilityExecute(this,*choiceAbilities[effectChoice-1]);
                } else {
                    std::cout << "This card does not have an expendChoice ability.\n";
                }
                break;
            case 4:
                if(chosenCard->hasAbilitiesWithTrigger(Trigger::Sacrifice)){
                    bool sacrificed = true;
                    for(auto& ab : chosenCard->abilities()){
                        if (ab.trigger == Trigger::Sacrifice){
                            if(!ab.used){
                                Game::smartAbilityExecute(this,ab);
                            } else {
                                std::cout << "This ability has already been used !\n";
                                sacrificed = false;
                                break;
                            }
                        }
                    }
                    if(sacrificed){
                        Game::sacrifice(chosenCard);
                        for (std::vector<Champion*>::iterator it = champions_.begin(); it != champions_.end();)
                        {
                            // *it sert à récupérer l'objet Card pointé par l'itérateur
                            if (champions_[cardChoice-1]->id() == (*it)->id()){
                                it = champions_.erase(it);
                                break;
                            } else {
                                    ++it;
                            }
                        }
                    }
                } else {
                    std::cout << "This card does not have a sacrifice ability.\n";
                }
                break;
            case 5:
                std::cout << "No ability played. Returning to action choice.\n";
                break;
        }
    } else {
        Card* chosenCard = inPlay_[cardChoice-champions_.size()-1];
                            
        std::cout << "The card you picked has the following abilities :\n";
        for(auto& ab : chosenCard->abilities()){
            ab.printAbility();
        }
        std::cout << "Which type of ability do you want to use ? (all the abilities with the same trigger will be activated at once.):\n";
        std::cout << "1. Ally - 2. Expend - 3. ExpendChoice - 4. Sacrifice - 5. Return.\n";

        int typeChoice;
        while(!(std::cin >> typeChoice) || typeChoice < 1 || typeChoice > 5){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch(typeChoice){
            case 1:
                if(chosenCard->hasAbilitiesWithTrigger(Trigger::Ally)){
                    if(isFactionInPlayExclude(chosenCard->faction(),chosenCard)){
                        for(auto& ab : chosenCard->abilities()){
                            if (ab.trigger == Trigger::Ally){
                                if(!ab.used){
                                    Game::smartAbilityExecute(this,ab);
                                } else {
                                    std::cout << "This ability has already been used !\n";
                                    break;
                                }
                            }
                        }
                    } else {
                        std::cout << "This card cannot activate its ally ability, there are no other cards of the same faction on your board.\n";
                    }
                } else {
                    std::cout << "This card does not have an ally ability.\n";
                }
                break;
            case 2:
                if(chosenCard->hasAbilitiesWithTrigger(Trigger::Expend)){
                    for(auto& ab : chosenCard->abilities()){
                        if (ab.trigger == Trigger::Expend){
                            if(!ab.used){
                                Game::smartAbilityExecute(this,ab);
                            } else {
                                std::cout << "This ability has already been used !\n";
                                break;
                            }
                        } 
                    }
                } else {
                    std::cout << "This card does not have an expend ability.\n";
                }
                break;
            case 3:
                if(chosenCard->hasAbilitiesWithTrigger(Trigger::ExpendChoice)){
                    std::vector<Card::CardAbility*> choiceAbilities = {};
                    for(auto& ab : chosenCard->abilities()){
                        if (ab.trigger == Trigger::ExpendChoice){
                            if(!ab.used){
                                choiceAbilities.push_back(&ab);
                            } else {
                                std::cout << "This ability has already been used !\n";
                                break;
                            }
                        } 
                    }
                    std::cout << "Chose the effect you want to use :\n";
                    for (int i=0;i<(int)choiceAbilities.size();i++){
                        std::cout << " - " << i+1 << "\n";
                        choiceAbilities[i]->printAbility();
                        // On met toutes les capacités à used pour ne pas pouvoir utiliser chaque choix séparément.
                        choiceAbilities[i]->used = true;
                    }
                    int effectChoice;
                    while(!(std::cin >> effectChoice) || effectChoice < 1 || effectChoice > (int)choiceAbilities.size()){
                        std::cout << "Invalid input. Please enter a valid choice: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    Game::smartAbilityExecute(this,*choiceAbilities[effectChoice-1]);
                } else {
                    std::cout << "This card does not have an expendChoice ability.\n";
                }
                break;
            case 4:
                if(chosenCard->hasAbilitiesWithTrigger(Trigger::Sacrifice)){
                    bool sacrificed = true;
                    for(auto& ab : chosenCard->abilities()){
                        if (ab.trigger == Trigger::Sacrifice){
                            if(!ab.used){
                                Game::smartAbilityExecute(this,ab);
                            } else {
                                std::cout << "This ability has already been used !\n";
                                sacrificed = false;
                                break;
                            }
                        }
                    }
                    if(sacrificed){
                        Game::sacrifice(chosenCard);
                        for (std::vector<Card*>::iterator it = inPlay_.begin(); it != inPlay_.end();)
                        {
                            // *it sert à récupérer l'objet Card pointé par l'itérateur
                            if (inPlay_[cardChoice-champions_.size()-1]->id() == (*it)->id()){
                                it = inPlay_.erase(it);
                                break;
                            } else {
                                    ++it;
                            }
                        }
                    }
                } else {
                    std::cout << "This card does not have a sacrifice ability.\n";
                }
                break;
            case 5:
                std::cout << "No ability played. Returning to action choice.\n";
                break;
        }
    }
}

void Player::cleanup(){
    for (Champion* champion : champions_){
        for (auto& ab : champion->abilities()){
            ab.used = false;
        }
        champion->heal();
    }
    discardPile_.insert(discardPile_.end(), hand_.cbegin(), hand_.cend());
    discardPile_.insert(discardPile_.end(), inPlay_.cbegin(), inPlay_.cend());
    for (Card* card : discardPile_){
        for (auto& ab : card->abilities()){
            ab.used = false;
        }
    }
    hand_.clear();
    inPlay_.clear();
    gold_ = 0;
    combat_ = 0;
    nextActionBuyOnDeck = false;
    nextBuyInHand = false;
    nextBuyTopOfDeck = false;
}