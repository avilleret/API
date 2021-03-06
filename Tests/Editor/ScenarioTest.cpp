#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <iostream>

using namespace ossia;
using namespace std::placeholders;

class ScenarioTest : public QObject
{
    Q_OBJECT

    std::shared_ptr<time_constraint> main_constraint;
    std::vector<time_value> events_date;

    void main_constraint_callback(time_value position, time_value date, const state& element)
    {
        std::cout << "Main Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void first_constraint_callback(time_value position, time_value date, const state& element)
    {
        std::cout << "First Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void second_constraint_callback(time_value position, time_value date, const state& element)
    {
        std::cout << "Second Constraint : " << double(position) << ", " << double(date) << std::endl;
    }

    void event_callback(time_event::Status newStatus)
    {
        time_value date = main_constraint->getDate();

        switch (newStatus)
        {
            case time_event::Status::NONE:
            {
                std::cout << "Event NONE" << std::endl;
                break;
            }
            case time_event::Status::PENDING:
            {
                std::cout << "Event PENDING at " << double(date) << " ms" << std::endl;
                break;
            }
            case time_event::Status::HAPPENED:
            {
                std::cout << "Event HAPPENED at " << double(date) << " ms" << std::endl;
                events_date.push_back(date);
                break;
            }
            case time_event::Status::DISPOSED:
            {
                std::cout << "Event DISPOSED at " << double(date) << " ms" << std::endl;
                break;
            }
        }
    }

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
      auto scenar = std::make_shared<scenario>();
        QVERIFY(scenar != nullptr);

        QVERIFY(scenar->parent() == nullptr);

        QVERIFY(scenar->getStartTimeNode() != nullptr);

        QVERIFY(scenar->timeNodes().size() == 1);
        QVERIFY(scenar->timeConstraints().size() == 0);

        QVERIFY(scenar->getStartTimeNode()->getDate() == 0.);

        auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
        auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);
        auto start_event = *(scenar->getStartTimeNode()->emplace(
                               scenar->getStartTimeNode()->timeEvents().begin(),
                               e_callback));

        auto end_node = std::make_shared<time_node>();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), e_callback));
        auto constraint = time_constraint::create(mc_callback, *start_event, *end_event, 1000._tv, 1000._tv, 1000._tv);

        QVERIFY(end_node->getDate() == 1000._tv);
    }

    /*! test edition functions */
    void test_edition()
    {
        auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
        auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);

        auto scenar = std::make_shared<scenario>();

        auto start_node = scenar->getStartTimeNode();
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), e_callback));

        auto end_node = std::make_shared<time_node>();
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), e_callback));

        auto constraint = time_constraint::create(mc_callback, *start_event, *end_event, 1000._tv, 1000._tv, 1000._tv);

        scenar->addTimeConstraint(constraint);
        QVERIFY(scenar->timeConstraints().size() == 1);
        QVERIFY(scenar->timeNodes().size() == 2);

        scenar->removeTimeConstraint(constraint);
        QVERIFY(scenar->timeConstraints().size() == 0);
        QVERIFY(scenar->timeNodes().size() == 2);

        auto lonely_node = std::make_shared<time_node>();

        scenar->addTimeNode(lonely_node);
        QVERIFY(scenar->timeNodes().size() == 3);

        scenar->removeTimeNode(lonely_node);
        QVERIFY(scenar->timeNodes().size() == 2);
    }

    /*! test execution functions */
    //! \todo maybe a way to test many scenario would be to load them from a files
    void test_execution()
    {
        using namespace ossia;
        auto mc_callback = std::bind(&ScenarioTest::main_constraint_callback, this, _1, _2, _3);
        auto fc_callback = std::bind(&ScenarioTest::first_constraint_callback, this, _1, _2, _3);
        auto sc_callback = std::bind(&ScenarioTest::second_constraint_callback, this, _1, _2, _3);
        auto e_callback = std::bind(&ScenarioTest::event_callback, this, _1);

        auto main_start_node = std::make_shared<time_node>();
        auto main_end_node = std::make_shared<time_node>();
        auto main_start_event = *(main_start_node->emplace(main_start_node->timeEvents().begin(), e_callback));
        auto main_end_event = *(main_end_node->emplace(main_end_node->timeEvents().begin(), e_callback));
        main_constraint = time_constraint::create(mc_callback, *main_start_event, *main_end_event, 5000._tv, 5000._tv, 5000._tv);

        auto main_scenario = std::make_unique<scenario>();

        auto scenario_start_node = main_scenario->getStartTimeNode();

        auto first_end_node = std::make_shared<time_node>();
        auto first_start_event = *(scenario_start_node->emplace(scenario_start_node->timeEvents().begin(), e_callback));
        auto first_end_event = *(first_end_node->emplace(first_end_node->timeEvents().begin(), e_callback));
        auto first_constraint = time_constraint::create(fc_callback, *first_start_event, *first_end_event, 1500._tv, 1500._tv, 1500._tv);

        main_scenario->addTimeConstraint(first_constraint);

        auto second_end_node = std::make_shared<time_node>();
        auto second_end_event = *(second_end_node->emplace(second_end_node->timeEvents().begin(), e_callback));
        auto second_constraint = time_constraint::create(sc_callback, *first_end_event, *second_end_event, 2000._tv, 2000._tv, 2000._tv);

        main_scenario->addTimeConstraint(second_constraint);

        main_constraint->addTimeProcess(std::move(main_scenario));

        main_constraint->setSpeed(1._tv);
        main_constraint->setGranularity(50._tv);
        first_constraint->setSpeed(1._tv);
        first_constraint->setGranularity(25._tv);
        second_constraint->setSpeed(1._tv);
        second_constraint->setGranularity(25._tv);

        events_date.clear();
        main_constraint->start();

        while (main_constraint->getRunning())
            ;

        // check TimeEvents date
        QCOMPARE((int)events_date.size(), 3);
        QVERIFY(events_date[0] == Zero);
        QVERIFY(events_date[1] >= first_end_node->getDate());
        // todo QVERIFY(events_date[1] < (first_end_node->getDate() + main_constraint->getGranularity()));
        QVERIFY(events_date[2] >= first_end_node->getDate());
        // todo QVERIFY(events_date[2] < first_end_node->getDate() + main_constraint->getGranularity());
    }
};

QTEST_APPLESS_MAIN(ScenarioTest)

#include "ScenarioTest.moc"
