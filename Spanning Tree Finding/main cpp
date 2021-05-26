#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <chrono>



///// -----     GRAPH     ----- /////

struct edge {
    size_t from;
    size_t to;
    size_t weight;
    size_t id;

    explicit edge (
            const size_t from = std::numeric_limits<size_t>::max(),
            const size_t to = std::numeric_limits<size_t>::max(),
            const size_t weight = std::numeric_limits<size_t>::max(),
            const size_t id = std::numeric_limits<size_t>::max()
    ): from(from), to(to), weight(weight), id(id) {}
};

class unordered_graph {
public:
    explicit unordered_graph (const size_t vertices_count): _graph(vertices_count), _edges_cnt(0) {}

    size_t get_vertices_count () const { return _graph.size(); }
    size_t get_edges_count () const { return _edges_cnt / 2; }

    void add_edge (const size_t v1, const size_t v2, const size_t weight) {
        _graph[v1].emplace_back(v1, v2, weight, _edges_cnt ++);
        _graph[v2].emplace_back(v2, v1, weight, _edges_cnt ++);
    }

    /// Returns all the edges that are outgoing from the vertex v.
    const std::vector<edge>& get_edges (const size_t v) const { return _graph[v]; }

private:
    std::vector<std::vector<edge>> _graph;
    size_t _edges_cnt;
};


///// -----     DSU         ----- /////

class disjoined_set_union {
public:
    explicit disjoined_set_union (const size_t vertices_count): _parents(vertices_count) {
        for (size_t i = 0; i < vertices_count; i ++)
            _parents[i] = i;
    }

    size_t get_component_number (const size_t vertex) {
        return
                vertex != _parents[vertex] ?
                        _parents[vertex] = get_component_number(_parents[vertex]) :
                vertex;
    }

    void unite_components (const size_t v1, const  size_t v2) {
        const size_t
                id1 = get_component_number(v1),
                id2 = get_component_number(v2);

        if (id1 != id2)
            _parents[id1] = id2;
    }

    /// returns true if there are multiple components, false otherwise.
    bool multiple_components () {
        if (_parents.empty())
            return false;

        const size_t id = get_component_number(0);

        for (size_t i = 1; i < _parents.size(); i ++)
            if (get_component_number(i) != id)
                return true;

        return false;
    }

private:
    std::vector<size_t> _parents;
};


///// -----        PRIM       ----- /////

unordered_graph prim (const unordered_graph& g) {
    const size_t v = g.get_vertices_count();
    unordered_graph mst(v);

    std::vector<edge> ingoing_edge(v, edge());
    ingoing_edge[0].weight = 0;
    std::set<std::pair<size_t, size_t>> q; /// key = weight, value = vertex num
    q.emplace(0, 0);

    while (!q.empty()) {
        const size_t vertex = q.begin()->second;
        q.erase(q.begin());

        const auto& edges_from = mst.get_edges(ingoing_edge[vertex].from);
        if (
                ingoing_edge[vertex].weight != 0 &&
                ingoing_edge[vertex].weight != std::numeric_limits<size_t>::max() &&
                /// Check that there is no such edge in mst yet (important!):
                std::find_if(
                        edges_from.cbegin(),
                        edges_from.cend(),
                        [&vertex] (const edge& e) -> bool {
                            return e.to == vertex;
                        }
                ) == edges_from.cend()
                )
            mst.add_edge(
                    ingoing_edge[vertex].from,
                    ingoing_edge[vertex].to,
                    ingoing_edge[vertex].weight
            );

        for (const edge& e : g.get_edges(vertex)) {
            int to = e.to;
            size_t weight = e.weight;

            if (weight < ingoing_edge[to].weight) {
                q.erase(std::make_pair(ingoing_edge[to].weight, to));
                ingoing_edge[to] = e;
                q.insert(std::make_pair(e.weight, to));
            }
        }
    }

    return mst;
}


///// -----      KRUSKAL      ----- /////

unordered_graph kruskal (const unordered_graph& g) {
    const size_t v = g.get_vertices_count();
    unordered_graph mst(v); /// O(V)
    disjoined_set_union dsu(v); /// O(V)

    std::vector<edge> all_edges; /// O(1)

    /// O(E)
    for (size_t i = 0; i < v; i ++)
        for (const edge& e : g.get_edges(i))
            all_edges.push_back(e);

    /// O(E) =/= O(V), but O(log V) = O(log E)

    /// O(E*log(V))
    std::sort(
            all_edges.begin(),
            all_edges.end(),
            /// Here I use lambda function in order to implement custom comparator:
            [] (const edge& a, const edge& b) -> bool {
                return a.weight < b.weight;
            }
    );

    /// O(E * log V)
    for (const edge& e : all_edges)
        /// e.from is in the different component than e.to
        if (dsu.get_component_number(e.from) != dsu.get_component_number(e.to)) {
            /// unite component that stores e.from and component that stores e.to
            dsu.unite_components(e.from, e.to);

            mst.add_edge(e.from, e.to, e.weight);
        }

    return mst;
}


///// -----      BORUVKA      ----- /////

unordered_graph boruvka (const unordered_graph& g) {
    const size_t v = g.get_vertices_count();
    unordered_graph mst(v); /// O(V)
    disjoined_set_union dsu(v); /// O(V)

    while (true) {
        /// We perform iteration only if there are multiple components, so we unite them...
        if (!dsu.multiple_components()) /// O(V * log V)
            break;

        std::unordered_map<size_t, edge> min_edge_from_component;

        /// Iterate on all of the edges in the graph:
        for (size_t i = 0; i < v; i ++)
            for (const auto e : g.get_edges(i)) {
                const size_t id1 = dsu.get_component_number(e.from);
                const size_t id2 = dsu.get_component_number(e.to);

                const edge& current = min_edge_from_component[id1];

                /// If components ids are not the same, the edge connects different components
                if (id1 != id2 && (
                        /// haven't selected the edge yet
                        min_edge_from_component.find(id1) == min_edge_from_component.end() ||
                        e.weight < current.weight || /// edge can be updated
                        (e.weight == current.weight && e.id < current.id)
                ))
                    min_edge_from_component[id1] = e;
            }

        for (const auto it : min_edge_from_component) {
            const edge& current_edge = it.second;
            const auto& outgoing_edges = mst.get_edges(current_edge.from);

            const bool edge_doesnt_exist_yet =
                    std::find_if(
                            outgoing_edges.cbegin(),
                            outgoing_edges.cend(),
                            [&current_edge] (const edge& e) -> bool {
                                return e.to == current_edge.to;
                            }
                    ) == outgoing_edges.cend();

            if (edge_doesnt_exist_yet)
                mst.add_edge(current_edge.from, current_edge.to, current_edge.weight);

            dsu.unite_components(current_edge.from, current_edge.to);
        }
    }

    return mst;
}


/// ------ GENERATOR & TESTER ----- /////

struct generator {
    enum class mode : size_t { sparse, dense };

    static std::string get_mode_name (const mode& gen_mode) {
        switch (gen_mode) {
            case mode::sparse: return "sparse";
            case mode::dense: return "dense";
        }
    };

    static unordered_graph generate (const size_t vertices_count, const mode& gen_mode) {
        unordered_graph the_graph(vertices_count);

        std::random_device device;
        std::mt19937 random_generator(device());
        std::uniform_int_distribution<size_t> weight(100, 1000);

        double p = 1. / static_cast<double>(vertices_count);
        std::uniform_real_distribution<double> add_edge_or_not(0., 1.);

        for (size_t i = 0; i < vertices_count - 1; i ++)
            the_graph.add_edge(i, i + 1, weight(random_generator));

        for (size_t i = 0; i < vertices_count; i ++)
            for (size_t j = i + 2; j < vertices_count; j ++)
                if (
                        gen_mode == mode::dense ?
                        add_edge_or_not(random_generator) > p :
                        add_edge_or_not(random_generator) < p
                        )
                    the_graph.add_edge(i, j, weight(random_generator));

        return the_graph;
    }
};

struct tester {
    typedef std::function<unordered_graph(const unordered_graph&)> algorithm;

    struct test_failed : std::exception {
        explicit test_failed (std::string&& description):
                _description(std::string("TEST FAILED: ").append(description)) {}

        const char* what () const noexcept override { return _description.c_str(); }

    private:
        std::string _description;
    };

    explicit tester (std::vector<std::pair<algorithm, std::string>>&& algorithms_and_names) {
        _algorithms.reserve(algorithms_and_names.size());
        _names.reserve(algorithms_and_names.size());

        for (const auto& algorithm_and_name : algorithms_and_names) {
            _algorithms.emplace_back(algorithm_and_name.first);
            _names.emplace_back(algorithm_and_name.second);
        }
    }

    std::vector<size_t> test_and_benchmark (const unordered_graph& g) {
        std::vector<unordered_graph> trees;
        std::vector<size_t> tree_weights;
        std::vector<size_t> times;

        trees.reserve(_algorithms.size());
        tree_weights.reserve(_algorithms.size());
        times.reserve(_algorithms.size());

        for (const algorithm& algo : _algorithms) {
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            trees.emplace_back(algo(g));
            tree_weights.emplace_back(0);

            for (size_t v = 0; v < trees.back().get_vertices_count(); v ++)
                for (const edge& e : trees.back().get_edges(v))
                    tree_weights.back() += e.weight;

            times.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - start
            ).count());
        }

        /// Comparing the trees:

        for (size_t algo_num = 0; algo_num < _algorithms.size(); algo_num ++) {
            if (trees[algo_num].get_vertices_count() != g.get_vertices_count())
                throw test_failed("wrong vertices count");

            if (trees[algo_num].get_edges_count() != g.get_vertices_count() - 1)
                throw test_failed("wrong edges count");

            if (tree_weights[algo_num] != tree_weights[0])
                throw test_failed("wrong tree weight");
        }

        return times;
    }

    size_t algorithms_count () { return _algorithms.size(); }

    const std::vector<std::string>& get_names () { return _names; }

private:
    std::vector<algorithm> _algorithms;
    std::vector<std::string> _names;
};


int main () {
    tester t(std::vector<std::pair<tester::algorithm, std::string>>{
            { prim, "prim" },
            { kruskal, "kruskal" },
            { boruvka, "boruvka" }
    });

    std::vector<std::pair<typename generator::mode, size_t>> tests({
                                                                           { generator::mode::sparse, 10000 },
                                                                           { generator::mode::dense, 1000 }
                                                                   });

    for (size_t test_num = 0; test_num < tests.size(); test_num ++) {
        std::cout <<
                  "-------------------------------" << std::endl <<
                  "TEST " << (test_num + 1) << " STARTED, params: { " <<
                  "mode: " << generator::get_mode_name(tests[test_num].first) << ", " <<
                  "max vertices count: " << tests[test_num].second << " }" << std::endl << std::endl;

        std::vector<std::vector<std::pair<size_t, size_t>>> algorithm_times(t.algorithms_count());

        for (size_t v = 0; v < tests[test_num].second; v += tests[test_num].second / 25) {
            if (v == 0) {
                for (size_t algo = 0; algo < t.algorithms_count(); algo++)
                    algorithm_times[algo].emplace_back(v, 0);

                continue;
            }

            try {
                const auto times =
                        t.test_and_benchmark(generator::generate(v, tests[test_num].first));

                for (size_t algo = 0; algo < t.algorithms_count(); algo++)
                    algorithm_times[algo].emplace_back(v, times[algo]);
            } catch (const tester::test_failed& exception) {
                std::cout << exception.what() << std::endl;
                return 0;
            }
        }

        for (size_t algo = 0; algo < t.algorithms_count(); algo ++) {
            std::cout << "Time of " << t.get_names()[algo] << ":" << std::endl;

            std::cout << "vertices_count = [";

            for (size_t i = 0; i < algorithm_times[algo].size(); i ++)
                std::cout << (i != 0 ? ", " : "") << algorithm_times[algo][i].first;

            std::cout << "]" << std::endl;

            std::cout << "times = [";

            for (size_t i = 0; i < algorithm_times[algo].size(); i ++)
                std::cout << (i != 0 ? ", " : "") << algorithm_times[algo][i].second;

            std::cout << "]" << std::endl << std::endl;
        }

        std::cout << std::endl;
    }
    return 0;
}
