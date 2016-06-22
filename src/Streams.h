#ifndef RUST_STREAMS_H
#define RUST_STREAMS_H

namespace streams {

	template <typename DerivedStreamExtractor>
	struct StreamExtractor {
		// TODO: noexcept?
		auto get() {
			return static_cast<DerivedStreamExtractor*>(this)->get_impl();
		}

		// TODO: noexcept?
		bool advance() {
			return static_cast<DerivedStreamExtractor*>(this)->advance_impl();
		}
	};

	template <typename IteratorType>
	struct SequenceStreamExtractor : StreamExtractor<SequenceStreamExtractor<IteratorType>> {
		SequenceStreamExtractor(IteratorType b, IteratorType e) : current(b), next(b), begin(b), end(e) {}

		IteratorType current;
		IteratorType next;
		const IteratorType begin;
		const IteratorType end;

		auto get_impl() {
			return current;
		}

		bool advance_impl() {
			if (next != end) {
				current = next++;
				return true;
			} else {
				return false;
			}
		}
	};

	// TODO: templated size_t counter + static_assertion?
	template<typename ExtractorType>
	struct SkipFirstStreamExtractor : StreamExtractor<SkipFirstStreamExtractor<ExtractorType>> {
		SkipFirstStreamExtractor(ExtractorType extractor, size_t count) : source(extractor), skipCount(count) {}

		ExtractorType source;
		size_t skipCount;

		auto get_impl() {
			return source.get();
		}

		bool advance_impl() {
			while (skipCount != 0) {
				--skipCount;
				if (!source.advance()) {
					return false;
				}
			}
			return source.advance();
		}

	};

	template<typename ExtractorType>
	struct LimitStreamExtractor : StreamExtractor<LimitStreamExtractor<ExtractorType>> {
		LimitStreamExtractor(ExtractorType extractor, size_t count) : source(extractor), limit(count) {}

		ExtractorType source;
		size_t limit;

		auto get_impl() {
			return source.get();
		}

		bool advance_impl() {
			if (limit != 0) {
				--limit;
				return source.advance();
			}
			return false;
		}

	};


	template<typename ExtractorType, typename Predicate>
	struct FilterStreamExtractor : StreamExtractor<FilterStreamExtractor<ExtractorType, Predicate>> {
		FilterStreamExtractor(ExtractorType extractor, Predicate&& p) : source(extractor), predicate(std::forward<Predicate>(p)) {}

		ExtractorType source;
		Predicate predicate;

		auto get_impl() {
			return source.get();
		}

		bool advance_impl() {
			if (!source.advance()) {
				return false;
			}
			auto element = source.get();
			while (!predicate(*element)) {
				if (source.advance()) {
					element = source.get();
				} else {
					return false;
				}
			}
			return true;
		}

	};


	template<typename ExtractorType, typename MapFunc>
	struct MapStreamExtractor : StreamExtractor<MapStreamExtractor<ExtractorType, MapFunc>> {
		MapStreamExtractor(ExtractorType sourceExtractor, MapFunc&& m) : source(sourceExtractor), mapFunc(std::forward<MapFunc>(m)) {}

		ExtractorType source;
		MapFunc mapFunc;

		//using InputType = decltype(std::declval<ExtractorType>().get());
		//using OutputType = decltype(std::declval<Mapper>()(*std::declval<InputType>()))
		//OutputType value;

		// TODO: consider using shared_ptr here
		decltype(mapFunc(*std::declval<decltype(source.get())>())) value;

		auto get_impl() {
			value = mapFunc(*source.get());
			return &value;
		}

		bool advance_impl() {
			return source.advance();
		}

	};


	template<typename ExtractorType, typename PeekFunc>
	struct PeekStreamExtractor : StreamExtractor<PeekStreamExtractor<ExtractorType, PeekFunc>> {
		PeekStreamExtractor(ExtractorType sourceExtractor, PeekFunc&& p) : source(sourceExtractor), peekFunc(std::forward<PeekFunc>(p)) {}

		ExtractorType source;
		PeekFunc peekFunc;

		auto get_impl() {
			auto value = source.get();
			peekFunc(*value);
			return value;
		}

		bool advance_impl() {
			return source.advance();
		}

	};


	// TODO: extractors are copied every time...
	// (n+1)th extractor will copy a chain of n extractors 
	template<typename ExtractorType>
	struct BaseStreamInterface {
		ExtractorType extractor;
		using iterator_type = decltype(extractor.get());
		using value_type = typename std::remove_reference<typename std::remove_pointer<iterator_type>::type>::type;

		BaseStreamInterface(ExtractorType e) : extractor(e) {}

		// Intermediate Operations

		template<typename MapFunc>
		auto map(MapFunc&& mapper) {
			using Extractor = MapStreamExtractor<decltype(extractor), MapFunc>;
			return BaseStreamInterface<Extractor>(Extractor(extractor, std::forward<MapFunc>(mapper)));
		}

		template<typename Predicate>
		auto filter(Predicate&& predicate) {
			using Extractor = FilterStreamExtractor<decltype(extractor), Predicate>;
			return BaseStreamInterface<Extractor>(Extractor(extractor, std::forward<Predicate>(predicate)));
		}

		auto skipFirst(size_t count) {
			using Extractor = SkipFirstStreamExtractor<decltype(extractor)>;
			return BaseStreamInterface<Extractor>(Extractor(extractor, count));
		}

		auto limit(size_t count) {
			using Extractor = LimitStreamExtractor<decltype(extractor)>;
			return BaseStreamInterface<Extractor>(Extractor(extractor, count));
		}

		template<typename PeekFunc>
		auto peek(PeekFunc&& peekFunc) {
			using Extractor = PeekStreamExtractor<decltype(extractor), PeekFunc>;
			return BaseStreamInterface<Extractor>(Extractor(extractor, std::forward<PeekFunc>(peekFunc)));
		}

		// Terminal Operations  

		template<typename Callable>
		void forEach(Callable&& callable) {
			while (extractor.advance()) {
				callable(*extractor.get());
			}
		}

		size_t count() {
			size_t counter = 0;
			while (extractor.advance()) {
				++counter;
			}
			return counter;
		}

	};

	template<typename Container>
	auto from(const Container& container) {
		using Extractor = SequenceStreamExtractor<decltype(std::begin(container))>;
		return BaseStreamInterface<Extractor>(Extractor(std::begin(container), std::end(container)));
	}

	template<typename Container>
	auto from(const Container&& container) = delete; // currently disastrous

} // namespace streams

#endif // !RUST_STREAMS_H