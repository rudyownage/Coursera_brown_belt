#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "profile.h"

using namespace std;

struct Email {
    string from;
    string to;
    string body;
};

istream& operator>>(istream& input, Email& mail) {
    getline(input, mail.from);
    getline(input, mail.to);
    getline(input, mail.body);
    return input;
}

ostream& operator<<(ostream& output, const Email& mail) {
    output << mail.from << '\n';
    output << mail.to << '\n';
    output << mail.body << '\n';
    return output;
}

class Worker {
public:
    virtual ~Worker() = default;
    virtual void Process(unique_ptr<Email> email) = 0;
    virtual void Run() {
        throw logic_error("Unimplemented");
    }

protected:
    unique_ptr<Worker> next_w;
    void PassOn(unique_ptr<Email> email) const {
        if (next_w) {
            next_w->Process(move(email));
        }
    }

public:
    void SetNext(unique_ptr<Worker> next) {
        if (!next_w) {
            next_w = move(next);
        }
        else {
            next_w->SetNext(move(next));
        }
    }

};


class Reader : public Worker {
private:
    istream& stream;
public:
    explicit Reader(istream& input) : stream(input){}

    void Process(unique_ptr<Email> email) override {
    }

    void Run() override {
        Email cur;
        while (stream >> cur) {
            PassOn(make_unique<Email>(cur));
        }
    }
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;
private:
    Function pr;
public:
    explicit Filter(Function pr_) : pr(move(pr_)){}

    void Process(unique_ptr<Email> email) override {
        if (pr(*email)) {
            PassOn(move(email));
        }
    }
};


class Copier : public Worker {
private:
    string receiver;
public:
    explicit Copier(string r) : receiver(move(r)) {}
    void Process(unique_ptr<Email> email) override {
        if(receiver != email->to){
            auto copy = make_unique<Email>(*email);
            copy->to = receiver;
            PassOn(move(email));
            PassOn(move(copy));
        }
        else {
            PassOn(move(email));
        }
    }
};


class Sender : public Worker {
private:
    ostream& output;
public:
    explicit Sender(ostream& s) : output(s){}
    void Process(unique_ptr<Email> email) override {
        output << *email;
        PassOn(move(email));
    }
};



class PipelineBuilder {
private:
    unique_ptr<Worker> first;
public:
    explicit PipelineBuilder(istream& in) : first(make_unique<Reader>(in)){
    }

    PipelineBuilder& FilterBy(Filter::Function filter) {
        first->SetNext(make_unique<Filter>(move(filter)));     
        return *this;
    }

    PipelineBuilder& CopyTo(string recipient) {
        first->SetNext(make_unique<Copier>(move(recipient)));
        return *this;
    }

    PipelineBuilder& Send(ostream& out) {
        first->SetNext(make_unique<Sender>(out));
        return *this;
    }

    unique_ptr<Worker> Build() {
        return move(first);
    }
};


void TestSanity() {
    string input = (
        "erich@example.com\n"
        "richard@example.com\n"
        "Hello there\n"

        "erich@example.com\n"
        "ralph@example.com\n"
        "Are you sure you pressed the right button?\n"

        "ralph@example.com\n"
        "erich@example.com\n"
        "I do not make mistakes of that kind\n"
        );

    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email& email) {
        return email.from == "erich@example.com";
        });
    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
        "erich@example.com\n"
        "richard@example.com\n"
        "Hello there\n"

        "erich@example.com\n"
        "ralph@example.com\n"
        "Are you sure you pressed the right button?\n"

        "erich@example.com\n"
        "richard@example.com\n"
        "Are you sure you pressed the right button?\n"
        );

    ASSERT_EQUAL(expectedOutput, outStream.str());
}


int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    return 0;
}