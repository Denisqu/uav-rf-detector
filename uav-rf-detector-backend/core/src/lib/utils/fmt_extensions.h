#ifndef FMT_EXTENSIONS_H
#define FMT_EXTENSIONS_H

#include <fmt/format.h>
#include <fmt/core.h>
#include <grpcpp/support/string_ref.h>

namespace fmt {
template <> struct fmt::formatter<grpc::string_ref>: formatter<string_view> {
	auto format(grpc::string_ref ref, format_context& ctx) const {
		string_view view = string_view(ref.data(), ref.size());
		return formatter<string_view>::format(view, ctx);
	};
};
}  // namespace fmt

#endif //FMT_EXTENSIONS_H
