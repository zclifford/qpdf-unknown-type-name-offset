#include <qpdf/assert_test.h>

#include <qpdf/Pl_Buffer.hh>
#include <qpdf/Pl_Concatenate.hh>
#include <qpdf/Pl_Flate.hh>
#include <qpdf/QUtil.hh>
#include <iostream>

static void
pipeStringAndFinish(Pipeline* p, std::string const& str)
{
    p->write(QUtil::unsigned_char_pointer(str), str.length());
    p->finish();
}

int
main(int argc, char* argv[])
{
    Pl_Buffer b1("compressed");
    Pl_Flate deflate("compress", &b1, Pl_Flate::a_deflate);
    Pl_Concatenate concat("concat", &deflate);
    pipeStringAndFinish(&concat, "-one-");
    pipeStringAndFinish(&concat, "-two-");
    concat.manualFinish();

    auto b1_buf = b1.getBufferSharedPointer();
    Pl_Buffer b2("uncompressed");
    Pl_Flate inflate("uncompress", &b2, Pl_Flate::a_inflate);
    inflate.write(b1_buf->getBuffer(), b1_buf->getSize());
    inflate.finish();
    auto b2_buf = b2.getBufferSharedPointer();
    std::string result(reinterpret_cast<char*>(b2_buf->getBuffer()), b2_buf->getSize());
    if (result == "-one--two-") {
        std::cout << "concatenate test passed" << '\n';
    } else {
        std::cout << "concatenate test failed: " << result << '\n';
    }
    return 0;
}
