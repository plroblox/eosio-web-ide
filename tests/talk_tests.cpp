#include <boost/test/unit_test.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/permission_object.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/testing/tester.hpp>

using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

BOOST_AUTO_TEST_SUITE(talk_tests)

BOOST_AUTO_TEST_CASE(post) try {
    tester t{setup_policy::none};

    // Load contract
    t.create_account(N(talk));
    t.set_code(N(talk), read_wasm("talk.wasm"));
    t.set_abi(N(talk), read_abi("talk.abi").data());
    t.produce_block();

    // Create users
    t.create_account(N(john));
    t.create_account(N(jane));
    t.create_account(N(peter));
    t.create_account(N(tom));

    // Test "post" action
    t.push_action(
        N(talk), N(post), N(john),
        mutable_variant_object //
        ("id", 1)              //
        ("reply_to", 0)        //
        ("user", "john")       //
        ("content", "post 1")  //
        ("likes", 0)           // Add new field "likes"
        ("dislikes", 0)        //
    );
    t.push_action(
        N(talk), N(post), N(jane),
        mutable_variant_object //
        ("id", 2)              //
        ("reply_to", 0)        //
        ("user", "jane")       //
        ("content", "post 2")  //
        ("likes", 10)          //
        ("dislikes", 2)        //
    );
    t.push_action(
        N(talk), N(post), N(john),
        mutable_variant_object       //
        ("id", 3)                    //
        ("reply_to", 2)              //
        ("user", "john")             //
        ("content", "post 3: reply") //
        ("likes", 0)                 //
        ("dislikes", 12)             //
    );

    // Test "addlikes" action
    t.push_action(
        N(talk), N(addlikes), N(peter),
        mutable_variant_object //
        ("id", 3)              //
    );

    // Test "adddislikes" action
    t.push_action(
        N(talk), N(adddislikes), N(tom),
        mutable_variant_object //
        ("id", 3)              //
    );

    // Can't reply to non-existing message
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(post), N(john),
                mutable_variant_object       //
                ("id", 4)                    //
                ("reply_to", 99)             //
                ("user", "john")             //
                ("content", "post 3: reply") //
            );
        }(),
        fc::exception);

    // likes or dislikes can NOT be negative
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(post), N(john),
                mutable_variant_object       //
                ("id", 5)                    //
                ("reply_to", 0)             //
                ("user", "john")             //
                ("content", "post 3: reply") //
                ("likes", -1)                //
                ("dislikes", -2)             //
            );
        }(),
        fc::exception);

    // Nobody could not assume a different person's identity to post
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(post), N(john),
                mutable_variant_object       //
                ("id", 5)                    //
                ("reply_to", 0)             //
                ("user", "jane")             //
                ("content", "post 3: reply") //
                ("likes", -1)                //
                ("dislikes", -2)             //
            );
        }(),
        fc::exception);

    // could not like a non-existing post
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(addlikes), N(peter),
                mutable_variant_object       //
                ("id", 123)                  //
            );
        }(),
        fc::exception);


    // could not dislike a non-existing post
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(talk), N(adddislikes), N(tom),
                mutable_variant_object       //
                ("id", 456)                  //
            );
        }(),
        fc::exception);
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
