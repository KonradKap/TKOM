#include "Filler.h"

#include <boost/range/irange.hpp>

#include "parser/Parser.h"
#include "parser/Type.h"
#include "parser/ReferenceList.h"
#include "utility/print_container.h"
#include "utility/utility.h"

//using namespace std::placeholders;
//
//Filler::Filler(BinaryReader& reader, std::shared_ptr<Type> root) :
//    reader(reader),
//    root(root) {
//    }
//
//DataArray Filler::fill() {
//    DataArray data_root;
//    data_root.has_value = false;
//    data_root.parent = nullptr;
//    root->apply(data_root);
//    return data_root;
//}
//
//void Filler::fillNode(const DeclaredData& declared, DataArray& context) {
//    const auto count = context.resolveArgument(declared.count);
//    for (const auto index : boost::irange(0, count)) {
//        DataArray next{
//            declared.identifier, declared.type_name(), index, context.applied_arguments, &context, false};
//        next.resolveArguments(declared.type.lock()->arguments, declared.appliedArguments);
//        declared.type.lock()->apply(*this, next);
//        context.children.push_back(next);
//    }
//}
