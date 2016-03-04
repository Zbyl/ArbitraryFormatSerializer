
#include <arbitrary_format/serialize.h>

#include <arbitrary_format/binary_serializers/VectorSaveSerializer.h>
#include <arbitrary_format/binary_serializers/MemorySerializer.h>
#include <arbitrary_format/binary_formatters/endian_formatter.h>
#include <arbitrary_format/binary_formatters/string_formatter.h>

#include <iostream>

using namespace arbitrary_format;
using namespace arbitrary_format::binary;

struct Person {
    std::string name;
    std::string surname;
    int age;
};

bool operator==(const Person& person0, const Person& person1)
{
    return (person0.name == person1.name) && (person0.surname == person1.surname) && (person0.age == person1.age);
}

template<typename name_formatter, typename surname_formatter, typename age_formatter>
struct person_formatter
{
    template<typename TSerializer>
    void save(TSerializer& serializer, const Person& person) const
    {
        serialize<name_formatter>(serializer, person.name);
        serialize<surname_formatter>(serializer, person.surname);
        serialize<age_formatter>(serializer, person.age);
    }

    template<typename TSerializer>
    void load(TSerializer& serializer, Person& person) const
    {
        serialize<name_formatter>(serializer, person.name);
        serialize<surname_formatter>(serializer, person.surname);
        serialize<age_formatter>(serializer, person.age);
    }
};

template<typename name_formatter, typename surname_formatter, typename age_formatter>
struct person_formatter2 : public implement_save_load< person_formatter2<name_formatter, surname_formatter, age_formatter> >
{
    template<typename TSerializer>
    void save_or_load(TSerializer& serializer, Person& person) const
    {
        serialize<name_formatter>(serializer, person.name);
        serialize<surname_formatter>(serializer, person.surname);
        serialize<age_formatter>(serializer, person.age);
    }
};

Person serializePerson(const Person& person)
{
    using str_format = string_formatter< little_endian<1> >;
    using age_format = little_endian<2>;
    using person_format = person_formatter<str_format, str_format, age_format>;
    using person_format2 = person_formatter2<str_format, str_format, age_format>;

    VectorSaveSerializer saveSerializer;
    serialize<person_format>(saveSerializer, person);

    Person loadedPerson;
    MemoryLoadSerializer loadSerializer(saveSerializer.getData());
    serialize<person_format2>(loadSerializer, loadedPerson);

    return loadedPerson;
}

int main()
{
    Person person;
    person.name = "John";
    person.surname = "Doe";
    person.age = 23;

    auto loadedPerson = serializePerson(person);

    if (loadedPerson == person)
    {
        std::cout << "Serializer works!" << std::endl;
    }
    else
    {
        std::cout << "Serializer doesn't work..." << std::endl;
    }
}
