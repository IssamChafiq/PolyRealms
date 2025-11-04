#include "Player.hpp"
#include "Game.hpp"
#include "Abilities.hpp"
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>

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

bool Player::draw(int n){
    int cardsLeft = deck_.getDeckContents().size();
    int discardLeft = discardPile_.size();
    if(cardsLeft + discardLeft < n){
        // Cas où le deck ET la défausse n'ont pas assez de cartes au total on pioche tout ce qu'on peut
        std::cout << "Cannot draw " << n << " cards, we draw " << cardsLeft + discardLeft << " instead.\n";
        draw(cardsLeft + discardLeft);
    } else if(n >= cardsLeft){
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
        std::cout << "\n" << name_ << " has drawn " << n << " cards.\n";
    } else {
        // Je pioche normalement
        std::vector<Card*> drawn = deck_.draw(n);
        hand_.insert(hand_.end(), drawn.cbegin(), drawn.cend());
        std::cout << "\n" << name_ << " has drawn " << n << " cards.\n";
    }
    return true;
}

// Joue la carte passée en paramètre
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
        champions_.push_back(static_cast<Champion*>(card));
    } else {
        inPlay_.push_back(card);
    }
    std::cout << name_ << " played the following card:\n";
    card->printCardInfo();
}

// Fonction pour acheter, on met la carte à un endroit différent selon les capacités utilisées
bool Player::buy(Card* card, Market& market){
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
        std::cout << name_ << " bought the following card:\n";
        card->printCardInfo();
        return true;
    } else {
        std::cout << "\nNot enough gold to buy this card : it costs " << card->cost() << " gold and you have " << gold_ << ".\n";
        return false;
    }  
}

// Pour buy en godmode : les cartes vont toujours dans la main, et le market vend la carte différemment
bool Player::godmodeBuy(Card* card, Market& market){
    if(gold_ >= card->cost()){
        gold_ -= card->cost();
        hand_.push_back(card);
        if(!(card->name() == "Fire Gem")){
            market.godmodeSell(card);
        }
        std::cout << name_ << " bought the following card:\n";
        card->printCardInfo();
        return true;
    } else {
        std::cout << "\nNot enough gold to buy this card : it costs " << card->cost() << " gold and you have " << gold_ << ".\n";
        return false;
    }  
}

bool Player::discard(int amount){
    std::cout << "\n" << name_ << ", you have to discard " << amount << " cards :\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    for (int i=0;i<amount;i++){
        std::cout << "Chose a card to discard :\n";
        if ((int)hand_.size() > 0){

            for (int i=0;i<(int)hand_.size();i++){
                std::cout << " - " << i+1 << ":\n";
                hand_[i]->printCardInfo();
            }

            int discardChoice;
            while(!(std::cin >> discardChoice) || discardChoice < 1 || discardChoice > (int)hand_.size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            // On réinitialise les capacités de la carte
            for (auto& ab : hand_[discardChoice-1]->abilities()){
                ab.used = false;
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
            std::cout << "No cards left in hand : cannot discard anymore.\n";
            break;
        } 
    }
    return true;
}

// On donne au joueur la possibilité de sacrifier des cartes de sa main ou de sa défausse
bool Player::cardEffectSacrifice(int amount){
    std::cout << "\nYou can sacrifice " << amount << " cards :\n";
    for (int i=0;i<amount;i++){
        bool sacrificeDone = false;
        while(!sacrificeDone){
            std::cout << "What do you want to do ? 1. Sacrifice from hand - 2. Sacrifice from discard pile - 3. Do nothing.\n";

            int choice;
                while(!(std::cin >> choice) || choice < 1 || choice > 3){
                    std::cout << "Invalid input. Please enter a valid choice: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            if (choice == 3){
                sacrificeDone = true;
                return true;
            }

            switch(choice){
                case 1:
                    if ((int)hand_.size() > 0){
                        for (int i=0;i<(int)hand_.size();i++){
                            std::cout << " - " << i+1 << ":\n";
                            hand_[i]->printCardInfo();
                        }

                        int handSacrificeChoice;
                        while(!(std::cin >> handSacrificeChoice) || handSacrificeChoice < 1 || handSacrificeChoice > (int)hand_.size()){
                            std::cout << "Invalid input. Please enter a valid choice: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }

                        Game::sacrifice(hand_[handSacrificeChoice-1]); 

                        // Supprime la carte de la main
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
                        sacrificeDone = true;
                        return true;
                    } else {
                        std::cout << "No cards left in hand : cannot sacrifice. Please try again or give up on the sacrifice.\n";
                        continue;
                    }
                    break;
                case 2:
                    if ((int)discardPile_.size() > 0){
                        for (int i=0;i<(int)discardPile_.size();i++){
                            std::cout << " - " << i+1 << "\n";
                            discardPile_[i]->printCardInfo();
                        }

                        int discardSacrificeChoice;
                        while(!(std::cin >> discardSacrificeChoice) || discardSacrificeChoice < 1 || discardSacrificeChoice > (int)discardPile_.size()){
                            std::cout << "Invalid input. Please enter a valid choice: ";
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }

                        Game::sacrifice(discardPile_[discardSacrificeChoice-1]);

                        // Supprime la carte de la défausse
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
                        sacrificeDone = true;
                        return true;
                    } else {
                        std::cout << "No cards left in discard pile : cannot sacrifice. Please try again or give up on the sacrifice.\n";
                        continue;
                    }
                    break;
                default: return false;
            }
        }
    }
    return false;
}

bool Player::prepareFriendlyChampion(){
    std::vector<Champion*> unprepared = {};
    // On récupère la liste des champions qui ont des capacités expend utilisées
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
        std::cout << "You do not have a champion you can prepare.\n";
        return false;
    } else {
        std::cout << "Which champion do you want to prepare ? :\n";
        for (int i=0;i<(int)unprepared.size();i++){
            std::cout << " - " << i+1 << ":\n";
            unprepared[i]->printCardInfo();
        }

        int prepareChoice;
        while(!(std::cin >> prepareChoice) || prepareChoice < 1 || prepareChoice > (int)unprepared.size()){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        // On prépare le champion choisi, en remettant à false toutes ses capacités expend utilisées
        for (Champion* champion : champions_){
            if(champion->id() == unprepared[prepareChoice-1]->id()){
                for (auto& ab : champion->abilities()){
                    if(ab.trigger == Trigger::Expend || ab.trigger == Trigger::ExpendChoice){
                        if(ab.used){
                            ab.used = false;
                        }
                    }
                }
                std::cout << name_ << " prepared the following champion :\n";
                champion->printCardInfo();
            }
        }
    }
    return true;
}

// Permet au joueur de choisir un champion dans sa défausse et de le mettre en haut de son deck
bool Player::getChampionFromDiscardToDeck(){
    std::vector<Champion*> championsDiscard = {};
    for (Card* card : discardPile_){
        if(card->isChampion()){
            championsDiscard.push_back(static_cast<Champion*>(card));
        }
    }
    if(championsDiscard.size() == 0){
        std::cout << "There are no champions in the discard pile\n";
        return false;
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

        // On réinitialise les capacités du champion
        for (auto& ab : championsDiscard[choice-1]->abilities()){
            ab.used = false;
        }
        
        championsDiscard[choice-1]->unStun();

        deck_.addToTop(championsDiscard[choice-1]);
        // Supprime la carte de la défausse
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
    return true;
}

// On donne au joueur la possibilité de choisir une carte dans sa défausse à mettre en haut de son deck
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
            std::cout << "There are no cards in the discard pile\n";
        } else {
            std::cout << "Which card do you want to bring to the top of your deck ?\n";
            for (int i=0;i<(int)discardPile_.size();i++){
                std::cout << " - " << i+1 << ":\n";
                discardPile_[i]->printCardInfo();
            }
            int choice;
            while(!(std::cin >> choice) || choice < 1 || choice > (int)discardPile_.size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            // On réinitialise les capacités de la carte
            for (auto& ab : discardPile_[choice-1]->abilities()){
                ab.used = false;
            }
            if(discardPile_[choice-1]->isChampion()){
                static_cast<Champion*>(discardPile_[choice-1])->unStun();
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
            
            std::cout << "Card added to the top of the deck :\n";
            discardPile_[choice-1]->printCardInfo();
        }
    } else if (drawChoice == 2){
        std::cout << "No card taken from your discard pile.\n";
    } 
}

// Vérifie si le joueur a au moins une carte d'une faction spécifique en jeu
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

// Même fonction que isFactionInPlay, mais on exclut une carte spécifique (utile si la carte est déjà en jeu)
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

// Vérifie si le joueur possède au moins un garde en jeu
bool Player::isGuarded(){
    for (Champion* champion : champions_){
        if (champion->isGuard()){
            return true;
        }
    }
    return false;
}

bool Player::stunChampion(){
    // Pour ne pas rester bloqué si l'adversaire n'a pas de champion
    if(champions_.size() == 0){
        std::cout << "This player doesnt have a stunnable champion.\n";
        return false;
    } else {
        // Si le joueur est gardé, on ne peut stun que ses gardes
        if(isGuarded()){
            std::vector<Champion*> guards = getGuards();
            std::cout << "This opponent is guarded, you can only stun his guards. Which guard do you want to stun ? :\n";
            for (int i=0;i<(int)guards.size();i++){
                std::cout << " - " << i+1 << ":\n";
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
                if (guards[guardChoice-1]->id() == (*it)->id()){
                    it = champions_.erase(it);
                    break;
                } else {
                        ++it;
                }
            }
        // Sinon, on peut stun n'importe quel champion
        } else {
            std::cout << "Which champion do you want to stun ? :\n";
            for (int i=0;i<(int)champions_.size();i++){
                std::cout << " - " << i+1 << ":\n";
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
                if (champions_[championChoice-1]->id() == (*it)->id()){
                    it = champions_.erase(it);
                    break;
                } else {
                        ++it;
                }
            }
        }
    }
    return true;
}

void Player::attack(Player* player, bool toRight){
    // Si le joueur ciblé est gardé, on ne peut attaquer que ses gardes
    if(player->isGuarded()){
        std::vector<Champion*> guards = player->getGuards();
        std::cout << "\n+------------------------------------------------------------+\n";
        std::cout << "|  \033[1m" << player->getName() << " is guarded.\033[0m You can only attack their guards!\n";
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "\033[1mWhich guard do you want to attack?\033[0m\n";
        std::cout << "--------------------------------------------------------------\n";

        for (int i=0;i<(int)guards.size();i++){
            std::cout << " - " << i+1 << ":\n";
            guards[i]->printCardInfo();
        }
        
        std::cout << "Enter your choice: ";
        int guardChoice;
        while(!(std::cin >> guardChoice) || guardChoice < 1 || guardChoice > (int)guards.size()){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if(guards[guardChoice-1]->takeDamage(combat_)){
            combat_ = combat_ - guards[guardChoice-1]->getMaxShield();
            player->discardPile_.push_back(guards[guardChoice-1]);
            player->removeChampion(guards[guardChoice-1]);
        } else {
            std::cout << "Not enough combat to kill this guard.\n";
        }
    // Sinon, on peut choisir d'attaquer le joueur ou ses champions
    } else {
        std::cout << "\n+------------------------------------------------------------+\n";
        std::cout << "|  \033[1mWhat do you want to attack?\033[0m\n";
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "|  [1] " << player->getName() << " (" << player->getAuthority() << " health points)\n";
        std::cout << "|  [2] Their Champions\n";
        std::cout << "|  [3] Return\n";
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "\033[1mEnter your choice:\033[0m ";

        int choice;
        while(!(std::cin >> choice) || choice < 1 || choice > 3){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        if (choice == 1){
            // Pour les modes où on ne peut qu'attaquer à gauche, on interdit d'attaquer le joueur de droite (toRight ne vaut true que dans ces modes).
            if(toRight){
                std::cout << "You can only attack the player to your left in this mode. You can however attack the champions of this player.\n";
            } else {
                int dmg = std::min(combat_, player->getAuthority());
                player->setAuthority(player->getAuthority() - dmg);
                combat_ -= dmg;
                std::cout << "You attacked " << player->getName() << " for " << dmg << " damage !\n";
                std::cout << player->getName() << " now has " << player->getAuthority() << " health left.\n";
            }
        } else if (choice == 2){
            // Pour ne pas rester bloqué si le joueur n'a pas de champion
            if((int)player->getChampions().size() == 0){
                std::cout << "This player doesnt have any champions.\n";
                return;
            }
            for (int i=0;i<(int)player->getChampions().size();i++){
                std::cout << " - " << i+1 << ":\n";
                player->getChampions()[i]->printCardInfo();
            }
            std::cout << "\n";

            int championChoice;
            while(!(std::cin >> championChoice) || championChoice < 1 || championChoice > (int)player->getChampions().size()){
                std::cout << "Invalid input. Please enter a valid choice: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            // On ne peut pas tuer un champion si on a pas assez de combat pour le faire en un coup
            if(player->getChampions()[championChoice-1]->takeDamage(combat_)){
                combat_ = combat_ - player->getChampions()[championChoice-1]->getMaxShield();
                player->discardPile_.push_back(player->getChampions()[championChoice-1]);
                player->removeChampion(player->getChampions()[championChoice-1]);
            } else {
                std::cout << "Not enough combat to kill this champion\n";
            }
        } else if (choice == 3){
            return;
        }
    }
}

void Player::useAbility(int cardChoice){
    if(cardChoice <= (int)champions_.size()){

        Champion* chosenCard = champions_[cardChoice-1];
        
        // On affiche les capacités de la carte choisie
        std::cout << "\n+------------------------------------------------------------+\n";
        std::cout << "|  \033[1mThe card you picked has the following abilities:\033[0m\n";
        std::cout << "+------------------------------------------------------------+\n";
        for (auto& ab : chosenCard->abilities()) {
            ab.printAbility();
        }
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "|  \033[1mSelect the type of ability to activate\033[0m\n";
        std::cout << "|  (All abilities with the same trigger activate together)\n";
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "|  [1] Ally\n";
        std::cout << "|  [2] Expend\n";
        std::cout << "|  [3] ExpendChoice\n";
        std::cout << "|  [4] Sacrifice\n";
        std::cout << "|  [5] Return\n";
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "\033[1mEnter your choice:\033[0m ";

        int typeChoice;
        while(!(std::cin >> typeChoice) || typeChoice < 1 || typeChoice > 5){
            std::cout << "Invalid input. Please enter a valid choice: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        // Différentes actions selon le type de capacité choisie
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
                    std::cout << "\nChose the effect you want to use :\n";

                    for (int i=0;i<(int)choiceAbilities.size();i++){
                        std::cout << " - " << i+1 << "\n";
                        choiceAbilities[i]->printAbility();
                    }

                    int effectChoice;
                    while(!(std::cin >> effectChoice) || effectChoice < 1 || effectChoice > (int)choiceAbilities.size()){
                        std::cout << "Invalid input. Please enter a valid choice: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }

                    // smartAbilityExecute retourne true si l'ability a bien été jouée, on peut ensuite mettre les autres options à used
                    if(Game::smartAbilityExecute(this,*choiceAbilities[effectChoice-1])){
                        for (int i=0;i<(int)choiceAbilities.size();i++){
                            // On met toutes les capacités à used pour ne pas pouvoir utiliser chaque choix séparément.
                            choiceAbilities[i]->used = true;
                        }
                    }
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
                                // Ne devrait pas être possible, mais juste au cas où
                                std::cout << "This ability has already been used !\n";
                                sacrificed = false;
                                break;
                            }
                        }
                    }
                    // Normalement toujours true car une carte sacrifiée ne peut pas être utilisée à nouveau
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
        // Même principe que pour les champions, mais pour les cartes en jeu. On prend un index réduit pour accéder à la bonne carte étant donné que les champions sont listés en premier.
        Card* chosenCard = inPlay_[cardChoice-champions_.size()-1];
                            
        std::cout << "\n+------------------------------------------------------------+\n";
        std::cout << "|  \033[1mThe card you picked has the following abilities:\033[0m\n";
        std::cout << "+------------------------------------------------------------+\n";
        for (auto& ab : chosenCard->abilities()) {
            ab.printAbility();
        }
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "|  \033[1mSelect the type of ability to activate\033[0m\n";
        std::cout << "|  (All abilities with the same trigger activate together)\n";
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "|  [1] Ally\n";
        std::cout << "|  [2] Expend\n";
        std::cout << "|  [3] ExpendChoice\n";
        std::cout << "|  [4] Sacrifice\n";
        std::cout << "|  [5] Return\n";
        std::cout << "+------------------------------------------------------------+\n";
        std::cout << "\033[1mEnter your choice:\033[0m ";


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
                    for (int i=0;i<(int)choiceAbilities.size();i++){
                            std::cout << " - " << i+1 << "\n";
                            choiceAbilities[i]->printAbility();
                    }

                    std::cout << "\nChose the effect you want to use :\n";
                    int effectChoice;
                    while(!(std::cin >> effectChoice) || effectChoice < 1 || effectChoice > (int)choiceAbilities.size()){
                        std::cout << "Invalid input. Please enter a valid choice: ";
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    
                    if(Game::smartAbilityExecute(this,*choiceAbilities[effectChoice-1])){
                        for (int i=0;i<(int)choiceAbilities.size();i++){
                            // On met toutes les capacités à used pour ne pas pouvoir utiliser chaque choix séparément.
                            choiceAbilities[i]->used = true;
                        }
                    }
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

// Fonction qui gère la phase de défausse de la partie. On nettoie la main et les cartes en jeu et on réinitialise les variables du joueur. On soigne également les champions (même si ils ne devraient pas être blessés techniquement, on ne peut que les tuer en un coup).
void Player::cleanup(){
    for (Champion* champion : champions_){
        for (auto& ab : champion->abilities()){
            ab.used = false;
        }
        champion->unStun();
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