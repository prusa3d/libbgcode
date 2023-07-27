#ifndef LIBBGCODE_HPP
#define LIBBGCODE_HPP

namespace bgcode {

class AbstractInterface {
public:

    virtual ~AbstractInterface() = default;

    virtual int foo() = 0;
};

} // namespace bgcode

#endif // LIBBGCODE_HPP
