#include <eosio/eosio.hpp>
#include <string>

// Message table
struct [[eosio::table("message"), eosio::contract("talk")]] message {
    uint64_t    id       = {}; // Non-0
    uint64_t    reply_to = {}; // Non-0 if this is a reply
    eosio::name user     = {};
    std::string content  = {};

    // Here the type of likes is signed int64_t, because during check(), we want to make sure
    // negative number such as -1 won't make a huge postive with implicit cast uint64_t(-1)
    int64_t    likes    = {};
    int64_t    dislikes = {};

    uint64_t primary_key() const { return id; }
    uint64_t get_reply_to() const { return reply_to; }
};

using message_table = eosio::multi_index<
    "message"_n, message, eosio::indexed_by<"by.reply.to"_n, eosio::const_mem_fun<message, uint64_t, &message::get_reply_to>>>;

// The contract
class talk : eosio::contract {
  public:
    // Use contract's constructor
    using contract::contract;

    // Post a message
    [[eosio::action]] void post(uint64_t id, uint64_t reply_to, eosio::name user, const std::string& content, int64_t likes, int64_t dislikes) {
        message_table table{get_self(), 0};

        // Check user
        require_auth(user);

        // Check reply_to exists
        if (reply_to)
            table.get(reply_to);

         // Make sure likes/dislike are non-negative
        eosio::check(likes >= 0, "#likes should never be negative!");
        eosio::check(dislikes >= 0, "#dislikes should never be negative!");

        // Create an ID if user didn't specify one
        eosio::check(id < 1'000'000'000ull, "user-specified id is too big");
        if (!id)
            id = std::max(table.available_primary_key(), 1'000'000'000ull);

        // Record the message
        table.emplace(get_self(), [&](auto& message) {
            message.id       = id;
            message.reply_to = reply_to;
            message.user     = user;
            message.content  = content;
            message.likes    = likes;
            message.dislikes = dislikes;
        });
    }

    //Add likes by 1 from any user
    [[eosio::action]] void addlikes(uint64_t id) {
        message_table table{get_self(), 0}; //Here vraible table is a multi_index
        auto itr =table.find(id);
        std::string strErr = std::string("addlikes() failed! Message id #") + std::to_string(id) + " does NOT exist in table";
        eosio::check(itr!= table.end(), strErr.c_str());
        table.modify(itr, get_self(), [&] (auto& message) {
                message.likes++;
            });

    }

    //Add dislikes by 1 from any user
    [[eosio::action]] void adddislikes(uint64_t id) {
        message_table table{get_self(), 0};
        auto itr =table.find(id);
        std::string strErr = std::string("adddislikes() failed! Message id #") + std::to_string(id) + " does NOT exist in table";
        eosio::check(itr!= table.end(), strErr.c_str());
        table.modify(itr, get_self(), [&] (auto& message) {
                message.dislikes++;
            });

    }
};
