#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

class time_valueTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        time_value zero;
        QVERIFY(zero == 0.);
        QVERIFY(zero == Zero);
        QVERIFY(zero.isInfinite() == false);

        time_value one(1.);
        QVERIFY(one == 1.);
        QVERIFY(one == One);
        QVERIFY(one.isInfinite() == false);

        time_value infinite(INFINITY);
        QVERIFY(infinite == Infinite);
        QVERIFY(infinite.isInfinite() == true);
    }

    /*! test =, +, +=, -, -= operators */
    void test_math()
    {
        time_value zero;
        time_value one(1.);
        time_value infinite(INFINITY);

        time_value copy = one;
        QVERIFY(copy == 1.);

        copy = infinite;
        QVERIFY(copy == INFINITY);

        QVERIFY(zero + one == 1.);
        QVERIFY(one + infinite == INFINITY);

        QVERIFY(one - one == 0.);
        QVERIFY(infinite - one == INFINITY);
    }

    /*! test comparison operators */
    void test_comparison()
    {
        time_value five(5.);
        time_value ten(10.);

        QVERIFY(five == five);
        QVERIFY(!(five == 10.));
        QVERIFY(!(five == ten));
        QVERIFY(!(five == Infinite));
        QVERIFY(!(Infinite == 10.));
        QVERIFY(!(Infinite == ten));

        QVERIFY(five != ten);
        QVERIFY(five != 10.);
        QVERIFY(five != Infinite);
        QVERIFY(Infinite != ten);
        QVERIFY(Infinite != 10.);
        QVERIFY(!(five != five));

        QVERIFY(five < 10.);
        QVERIFY(five < ten);
        QVERIFY(five <= 10.);
        QVERIFY(five <= ten);
        QVERIFY(ten > 5.);
        QVERIFY(ten > five);
        QVERIFY(ten >= 5.);
        QVERIFY(ten >= five);

        QVERIFY(Infinite > five);
        QVERIFY(Infinite > 5.);
        QVERIFY(Infinite >= five);
        QVERIFY(Infinite >= 5.);
        QVERIFY(five < Infinite);
        QVERIFY(five <= Infinite);

        QVERIFY(!(Infinite > Infinite));
        QVERIFY(Infinite >= Infinite);
        QVERIFY(!(Infinite < Infinite));
        QVERIFY(Infinite <= Infinite);

        QVERIFY(!(0. == Infinite));
        QVERIFY(0. != Infinite);
        QVERIFY(5. < Infinite);
        QVERIFY(5. <= Infinite);
    }
};

QTEST_APPLESS_MAIN(time_valueTest)

#include "TimeValueTest.moc"
