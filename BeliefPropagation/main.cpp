#include <cstdlib>
#include <cmath>
#include <vector>
#include <assert.h>
#include <fstream>
#include <boost/random.hpp>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <queue>

#include "../mem.hpp"

//#define USE_SPLASH 1
//#define OUTPUT_DATA 1
#define USE_STRUCTS 1
static size_t GRID_SIZE(10);
//#define OUTPUT_IMAGES 1
//#define OUTPUT_RESULTS 1

using namespace std;

static double bound = 1E-4;

class size_heap {
private:
    
    typedef pair<size_t, double> obj;
    
    typedef vector<obj> vec_objects;
    
    vec_objects objects;
    
    vector<size_t> index_map;
    
    size_t BLANK;
    
    int left(int parent)
    {
        return 2 * parent;
    }
    
    int right(int parent)
    {
        return 2 * parent + 1;
    }
    
    int parent(int child)
    {
        return child / 2;
    }
    
    bool less(size_t i, size_t j)
    {
        assert(i < objects.size());
        assert(j < objects.size());
        return objects[i].second < objects[j].second;
    }
    
    double priority_at(size_t i)
    {
        return objects[i].second;
    }
    
    void swap(int a, int b)
    {
        if(a == b) return;
        std::swap(objects[a], objects[b]);
        assert(objects[a].first < index_map.size());
        assert(objects[b].first < index_map.size());
        index_map[objects[a].first] = a;
        index_map[objects[b].first] = b;
    }
    
    void heapify(size_t i)
    {
        size_t l = left(i);
        size_t r = right(i);
        size_t s = size();
        size_t largest = i;
        
        if((l <= s) && less(i, l))
            largest = l;
        if((r <= s) && less(largest, r))
            largest = r;
        if(largest != i) {
            swap(i, largest);
            heapify(largest);
        }
    }
    
public:
    
    size_t size(void) const { return objects.size() - 1; }
    inline bool empty(void) const { return size() == 0; }
    
    size_heap(void):
    objects(1, std::make_pair(-1, double())), BLANK(-1)
    {
    }
    
    ~size_heap(void)
    {
        
    }
    
    const pair<size_t, double>& top() const
    {
        assert(objects.size() > 1);
        return objects[1];
    }
    
    pair<size_t, double> pop()
    {
        assert(size() > 0);
        obj top = objects[1];
        assert(top.first < index_map.size());
        swap(1, size());
        objects.pop_back();
        heapify(1);
        index_map[top.first] = BLANK;
        return top;
    }
    
    double get(size_t item) const
    {
        assert(item < index_map.size());
        assert(index_map[item] != BLANK);
        return objects[index_map[item]].second;
    }
    
    double operator[](size_t item) const
    {
        return get(item);
    }
    
    void push(size_t element, const double priority)
    {
        assert(element != BLANK);
        objects.push_back(std::make_pair(element, priority));
        size_t i = size();
        
        if(!(element < index_map.size())) {
            index_map.resize(element + 1, BLANK);
        }
        
        index_map[element] = i;
        
        while((i > 1) && (priority_at(parent(i)) <= priority)) {
            swap(i, parent(i));
            i = parent(i);
        }
    }
    
    bool contains(const size_t& item) const
    {
        return item < index_map.size() && index_map[item] != BLANK;
    }
    
    void update(size_t item, double priority)
    {
        assert(item < index_map.size());
        size_t i = index_map[item];
        objects[i].second = priority;
        while((i > 1) && (priority_at(parent(i)) < priority)) {
            swap(i, parent(i));
            i = parent(i);
        }
        
        heapify(i);
    }
    
    void insert_max(size_t item, double priority)
    {
        assert(item != BLANK);
        if(!contains(item)) {
            push(item, priority);
        } else {
            double effective_priority = max(get(item), priority);
            update(item, effective_priority);
        }
    }
    
    void clear() {
        objects.clear();
        objects.push_back(std::make_pair(-1, double()));
        index_map.clear();
    }
    
    bool remove(size_t item) {
        if(contains(item)) {
            assert(size() > 0);
            assert(item < index_map.size());
            size_t i = index_map[item];
            assert(i != BLANK);
            swap(i, size());
            objects.pop_back();
            heapify(i);
            index_map[item] = BLANK;
            return true;
        } else
            return false;
    }
};

static vector<double> data;
static size_t rows = 0, cols = 0;
static size_t colors = 0;
static double sigma = 2;
static double damping = 0.1;
static double lambda = 2;
#ifdef USE_SPLASH
static int splash_size = 100;
static bool *active_set;
static vector<int> vmap;
static const size_t queue_multiple = 5;
static const size_t ncpus = 1;
static vector< size_heap > pqueues;
typedef vector<int> splash_type;
static vector<splash_type> splashes;
static vector<size_t> splash_index;
#endif

typedef enum {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
} direction_t;

class edge_data
{
public:
    int from_id;
    int to_id;
    int message_var;
    int old_message_var;
    vector<double> message;
    vector<double> old_message;
    direction_t dir;
};


class vertex_data
{
public:
    int id;
    bool in_queue;
    int potential_var;
    int belief_var;
    vector<double> belief;
    vector<double> potential;
    vector<edge_data*> edges; // out edges
    vector<edge_data*> in_edges;
    vector<int> neighbors;
};

static void bp_update(vertex_data&);
static void add_task(vertex_data&, const double);

class binary_factor
{
private:
    
    vector<double> data;
    size_t var1;
    size_t var2;
    size_t arity1;
    size_t arity2;
    
public:
    
    double& logP(size_t x1, size_t a1,
            size_t x2, size_t a2)
    {
        assert(a1 < arity1);
        assert(a2 < arity2);
        
        return data[a1 + a2 * arity1];
    }
    
    double& logP(size_t a1, size_t a2)
    {
        return data[a1 + a2 * arity1];
    }
    
    void set_as_laplace(const double lambda)
    {
        for(size_t i(0); i < arity1; ++i) {
            for(size_t j(0); j < arity2; ++j) {
                logP(i, j) = -std::abs(double(i) - double(j)) * lambda;
            }
        }
    }
    
    void output_as(const string& name) const
    {
        cout << name << "(A, [";
        for(size_t i(0); i < data.size(); ++i) {
            if(i > 0)
                cout << ", ";
            cout << data[i];
        }
        cout << "])\n";
    }
    
    explicit binary_factor(const size_t _var1 = 0,
            const size_t _arity1 = 0,
            const size_t _var2 = 0,
            const size_t _arity2 = 0):
    data(_arity1 * _arity2),
    var1(_var1), var2(_var2), arity1(_arity1), arity2(_arity2)
    {}
};

static inline size_t
vertex_id(const size_t i, const size_t j)
{
    assert(i < rows);
    assert(j < cols);
    return i*cols + j;
}

static inline double&
pixel(const size_t i, const size_t j)
{
    return data[vertex_id(i, j)];
}

static void paint_sunset(size_t num_rings) {
    assert(rows > 0);
    assert(cols > 0);
    
    colors = num_rings;
    
    const double center_r = rows / 2.0;
    const double center_c = cols / 2.0;
    const double max_radius = std::min(rows, cols) / 2.0;
    
    for(size_t r(0); r < rows; ++r) {
        for(size_t c(0); c < cols; ++c) {
            double distance = sqrt((r-center_r)*(r-center_r) + (c-center_c)*(c-center_c));
            
            if(r < rows / 2) {
                size_t ring =
                        static_cast<size_t>(std::floor(std::min(1.0, distance/max_radius) * (num_rings - 1)));
               
                pixel(r, c) = ring;
            } else {
                pixel(r, c) = 0;
            }
        }
    }
}

static inline void
save_image(const char *filename)
{
    assert(rows > 0 && cols > 0);
    
    std::ofstream os(filename);
    
    os << "P2" << std::endl
            << cols << " " << rows << std::endl
            << 255 << std::endl;
   
    double min = data[0];
    double max = data[0];
    
    for(size_t i(0); i < rows * cols; ++i) {
        min = std::min(min, data[i]);
        max = std::max(max, data[i]);
    }
    
    //cout << "Min " << min << " max " << max << endl;
    
    if(min == max) {
        assert(false); // the fuck?
        for(size_t r(0); r < rows; ++r) {
            for(size_t c(0); c < cols; ++c) {
                os << min;
                if(c != cols-1) os << "\t";
            }
            os << std::endl;
        }
    } else {
        for(size_t r(0); r < rows; ++r) {
            for(size_t c(0); c < cols; ++c) {
                int color = static_cast<int>(255.0 * (pixel(r, c) - min)/(max-min));
                os << color;
                if(c != cols-1) os << "\t";
            }
            os << std::endl;
        }
    }
    
    
    os.flush();
    os.close();
}

static void
corrupt_image(double sigma)
{
    // XXX revert this
    boost::lagged_fibonacci607 rng;
    //boost::mt19937 rng;
    //rng.seed(0);
    boost::normal_distribution<double> noise_model(0, sigma);
    
    for(size_t i(0); i < rows * cols;) {
        data[i++] += noise_model(rng);
    }
#if 0
    for(size_t i(0); i < rows; ++i) {
        cout << "[";
        for(size_t j(0); j < cols; ++j) {
            cout << pixel(i, j);
            if(j < cols - 1)
                cout << ", ";
        }
        cout << "]" << endl;
    }
#endif
}

static inline void
set_size(const size_t _rows, const size_t _cols)
{
    rows = _rows;
    cols = _cols;
    data.resize(rows * cols);
}

static edge_data*
corresponding_outedge(vertex_data& vdata, edge_data *in_edge)
{
    int from(in_edge->from_id);
    
    for(size_t i(0); i < vdata.edges.size(); ++i) {
        if(vdata.edges[i]->to_id == from)
            return vdata.edges[i];
    }
    
    return NULL;
}

static vector<vertex_data> vertices;

static void
make_times(vector<double>& v1, const vector<double>& v2)
{
    assert(v1.size() == v2.size());
    for(size_t asg(0); asg < v1.size(); ++asg)
        v1[asg] += v2[asg];
}

static void
make_divide(vector<double>& v1, const vector<double>& v2)
{
    assert(v1.size() == v2.size());
    for(size_t asg(0); asg < v1.size(); ++asg)
        v1[asg] -= v2[asg];
}

static void
make_convolve(vector<double>& v1, binary_factor *factor, const vector<double>& v2)
{
    for(size_t x(0); x < v1.size(); ++x) {
        double sum = 0.0;
        for(size_t y(0); y < v2.size(); ++y)
            sum += std::exp(factor->logP(0, x, 0, y) + v2[y]);
        assert(!(sum < 0.0));
        if(sum == 0)
            sum = std::numeric_limits<double>::min();
        v1[x] = std::log(sum);
    }
}

static void
make_damp(vector<double>& v1, const vector<double>& v2, double damping)
{
    assert(v1.size() == v2.size());
    if(damping == 0) return;
    assert(damping >= 0.0);
    assert(damping < 1.0);
    for(size_t asg(0); asg < v1.size(); ++asg)
        v1[asg] = std::log(damping * std::exp(v2[asg]) + (1.0 - damping) * std::exp(v1[asg]));
}

static double
make_residual(const vector<double>& v1, const vector<double>& v2)
{
    assert(v1.size() == v2.size());
    double residual(0.0);
    for(size_t asg(0); asg < v1.size(); ++asg)
        residual += std::abs(std::exp(v1[asg]) - std::exp(v2[asg]));
    return residual / v1.size();
}

static void
make_uniform(vector<double>& v, const double value = 0.0)
{
    for(size_t i(0); i < v.size(); ++i) {
        v[i] = value;
    }
}

static int
make_max_asg(const vector<double>& v)
{
    size_t max_asg(0);
    double max_value = v[0];
    for(size_t asg(0); asg < v.size(); ++asg) {
        if(v[asg] > max_value) {
            max_value = v[asg];
            max_asg = asg;
        }
    }
    return max_asg;
}

static void
normalize(vector<double>& v)
{
    assert(v.size() > 0);
    double max_value(v[0]);
    for(size_t i(0); i < v.size(); ++i)
        max_value = std::max(max_value, v[i]);
    
    double Z = 0.0;
    for(size_t i(0); i < v.size(); ++i) {
        Z += std::exp(v[i] -= max_value);
    }
    
    assert(Z > 0.0);
    double logZ = std::log(Z);
    for(size_t i(0); i < v.size(); ++i)
        v[i] -= logZ;
}

static void
add_edge(const int from_id, const int to_id, const direction_t dir, edge_data& edata)
{
    assert(from_id < vertices.size() && to_id < vertices.size());
    edata.message_var = to_id;
    edata.old_message_var = edata.message_var;
    edata.to_id = to_id;
    edata.dir = dir;
    edge_data *n = new edge_data(edata);
    assert(n->from_id == edata.from_id);
    vertices[from_id].edges.push_back(n);
    vertices[to_id].in_edges.push_back(n);
    vertices[from_id].neighbors.push_back(to_id);
}

static void
construct_graph(void)
{
    double sigmaSq = sigma*sigma;
    vertex_data vdata;
    
    vdata.potential.resize(colors);
    vdata.belief.resize(colors);
    make_uniform(vdata.belief);
    make_uniform(vdata.potential);
    normalize(vdata.belief);
    normalize(vdata.potential);
    
    vdata.in_queue = false;
    
    for(size_t i(0); i < rows; ++i) {
        for(size_t j(0); j < rows; ++j) {
            int pixel_id = vertex_id(i, j);
            double obs = pixel(i, j);
            vdata.id = pixel_id;
            vdata.potential_var = vdata.belief_var = pixel_id;
            for(size_t pred(0); pred < colors; ++pred) {
                vdata.potential[pred] = -(obs - pred)*(obs - pred)/(2.0 * sigmaSq);
            }
            normalize(vdata.potential);
            
#if 0
            cout << i << " " << j << " ";
            for(size_t k(0); k < colors; ++k) {
                cout << vdata.potential[k] << " ";
            }
            cout << endl;
#endif
            
            assert(vertices.size() == vdata.id);
            vertices.push_back(vdata);
            assert(vertices.size() -1 == pixel_id);
        }
    }
    
    assert(vertices.size() == rows * cols);
    
    edge_data edata;
    edata.message.resize(colors);
    make_uniform(edata.message);
    normalize(edata.message);
    edata.old_message = edata.message;
    
    for(size_t i(0); i < rows; ++i) {
        for(size_t j(0); j < cols; ++j) {
            size_t vertid = vertex_id(i, j);
            edata.from_id = vertid;
            if(i-1 < rows) {
                add_edge(vertid, vertex_id(i - 1, j), TOP, edata);
            }
            if(i+1 < rows) {
                add_edge(vertid, vertex_id(i + 1, j), BOTTOM, edata);
            }
            if(j-1 < cols) {
                add_edge(vertid, vertex_id(i, j - 1), LEFT, edata);
            }
            if(j + 1 < cols) {
                add_edge(vertid, vertex_id(i, j + 1), RIGHT, edata);
            }
        }
    }
}

static void
print_data(void)
{
    for(size_t i(0); i < rows; ++i) {
        for(size_t j(0); j < rows; ++j) {
            vertex_data &v(vertices[vertex_id(i, j)]);
            
#if 0
            if(i == 0 && j == 0) {
                edge_data *edata(v.in_edges[0]);
                // print initial message
                cout << "initial-message(A, B, [";
                for(size_t i(0); i < edata->message.size(); ++i) {
                   if(i > 0)
                        cout << ", ";
                   cout << edata->message[i];
                }
                cout << "])\n";
                // print belief
                cout << "belief(A, [";
                for(size_t i(0); i < v.belief.size(); ++i) {
                    if(i > 0)
                        cout << ", ";
                    cout << v.belief[i];
                }
                cout << "])\n";
            }
#endif
            // print pixel
            //cout << "!pixel(@" << v.id << ", " <<
            //        i << ", " << j << ").\n";
            
            // potential is different for each node
            cout << "!potential(@" << v.id << ", ";
#ifdef USE_STRUCTS
            cout << ":(";
#else
            cout << "[";
#endif
            for(size_t k(0); k < v.potential.size(); ++k) {
                if(k > 0)
                    cout << ", ";
                cout << v.potential[k];
            }
#ifdef USE_STRUCTS
            cout << ")";
#else
            cout << "]";
#endif
            cout << ").\n";
       
            // print links
            for(size_t k(0); k < v.edges.size(); ++k) {
                edge_data *e(v.edges[k]);
                cout << "!edge(@" << v.id << ", @"
                        << e->to_id;
#if 0
                switch(v.edges[k]->dir) {
                    case TOP: cout << "0"; break;
                    case BOTTOM: cout << "1"; break;
                    case LEFT: cout << "2"; break;
                    case RIGHT: cout << "3"; break;
                    default: assert(false);
                }
#endif
                cout << ").\n";
            }
        }
    }
}

static size_t update_count(0);

static binary_factor *edge_potential(NULL);

static void
bp_update(vertex_data& vdata)
{
    assert(vdata.in_edges.size() == vdata.edges.size());
    
    for(vector<edge_data*>::iterator it(vdata.in_edges.begin()), end(vdata.in_edges.end());
            it != end;
            ++it)
    {
        edge_data *in_edge(*it);
        assert(in_edge != NULL);
        assert(in_edge->from_id < rows * cols);
        assert(in_edge->to_id < rows * cols);
        assert(in_edge->to_id != in_edge->from_id);
        in_edge->old_message = in_edge->message;
    }
    
    // compute belief
    vdata.belief = vdata.potential;
    for(vector<edge_data*>::iterator it(vdata.in_edges.begin()), end(vdata.in_edges.end());
            it != end;
            ++it)
    {
        edge_data *in_edge(*it);
        make_times(vdata.belief, in_edge->old_message);
    }
    normalize(vdata.belief);
    
    vector<double> cavity, tmp_msg;
    for(size_t i(0); i < vdata.in_edges.size(); ++i) {
        edge_data *in_edge(vdata.in_edges[i]);
        edge_data *out_edge(corresponding_outedge(vdata, in_edge));
        
        assert(in_edge->from_id == out_edge->to_id);
        assert(in_edge->to_id == out_edge->from_id);
        
        cavity = vdata.belief;
        make_divide(cavity, in_edge->old_message);
        normalize(cavity);
        
        tmp_msg.resize(out_edge->message.size(), 0.0);
        make_convolve(tmp_msg, edge_potential, cavity);
        normalize(tmp_msg);
        
        make_damp(tmp_msg, out_edge->message, damping);
        
        double residual = make_residual(tmp_msg, out_edge->old_message);
        
        out_edge->message = tmp_msg;
        
        if(residual > bound) {
            add_task(vertices[out_edge->to_id], residual);
        }
        
    }
}

static void
delete_graph(void)
{
    for(vector<vertex_data>::iterator it(vertices.begin()), end(vertices.end());
            it != end;
            ++it)
    {
        vertex_data *p = &(*it);
        for(size_t j(0); j < p->edges.size(); ++j)
            delete p->edges[j];
    }
}

static void
reconstruct_image(void)
{
    for(size_t i(0); i < vertices.size(); ++i) {
        vertex_data &vdata(vertices[i]);
        
        data[i] = make_max_asg(vdata.belief);
    }
}

static void
output_results(void)
{
    for(size_t i(0); i < vertices.size(); ++i) {
        vertex_data &vdata(vertices[i]);
        
        cout << "belief(@" << vdata.id << ", [";
        for(size_t j(0); j < vdata.belief.size(); ++j) {
            if(j != 0)
                cout << ", ";
            cout << vdata.belief[j];
        }
        cout << "])." << endl;
    }
}

#ifdef USE_SPLASH

static inline bool
active_set_set_bit(const int node_id)
{
    bool old = active_set[node_id];
    
    active_set[node_id] = true;
    
    return old;
}

static void
add_task(vertex_data &vdata, double priority)
{
    size_t node_id = (size_t)vdata.id;
    
    assert(node_id < vertices.size());
    int pqueue_id = vmap[node_id];
    size_heap &queue(pqueues[pqueue_id]);
    
    bool already_present = active_set_set_bit(node_id);
    
    if(!already_present ||
            queue.contains(node_id))
    {
        queue.insert_max(node_id, priority);
    }
}

static void
add_all_tasks_to_fifo(const double priority)
{
    for(vector<vertex_data>::iterator it(vertices.begin()), end(vertices.end());
            it != end;
            ++it)
    {
        add_task(*it, priority);
    }
}

static void
start_engine(void)
{
    active_set = new bool[vertices.size()];
    for(int i(0); i < vertices.size(); ++i)
        active_set[i] = false;
    vmap.resize(vertices.size());
    pqueues.resize(ncpus * queue_multiple);
    
    for(int i = 0; i < vmap.size(); ++i) {
      vmap[i] = i % pqueues.size();
    }
    
    splashes.resize(ncpus); // all the splashes
    splash_index.resize(ncpus, 0); // splash sizes
}

static bool
get_top(size_t cpuid, size_t& ret_vertex, double& ret_priority)
{
    static size_t lastqid[128] = {0}; // everything at 0
    
    for(size_t i = 0; i < queue_multiple; ++i) {
        size_t j = (i + lastqid[cpuid]) % queue_multiple;
        size_t index = cpuid * queue_multiple + j;
        
        size_heap& queue(pqueues[index]);
        
        if(!queue.empty()) {
            ret_vertex = queue.top().first;
            ret_priority = queue.top().second;
            queue.pop();
            lastqid[cpuid] = j + 1;
            return true;
        }
    }
    lastqid[cpuid] = 0;
    return false;
}

static size_t
work(size_t vertex)
{
    vertex_data& vdata = vertices[vertex];
    
    return vdata.in_edges.size() + vdata.edges.size();
}

static void
rebuild_splash(size_t cpuid)
{
    assert(cpuid < splashes.size());
    assert(cpuid == 0);
    splash_type& splash(splashes[cpuid]);
    
    assert(splash_index[cpuid] == splash.size());
    //assert(splash_index[cpuid] == 0);
    
    splash.clear();
    splash_index[cpuid] = 0;
    
    size_t root(-1);
    
    double root_priority(0);
    
    bool root_found = get_top(cpuid, root, root_priority);
    
    if(!root_found) { return; }
    
    splash.push_back(root);
    size_t splash_work = work(root);
    
    if(root_priority > 1) splash_work = splash_size;
    
    set<size_t> visited;
    queue<size_t> bfs_queue;
    
    visited.insert(root);
    
    // add root's neighbors
    vector<size_t> shuffled_edges;
    
    for(int i = 0; i < vertices[root].in_edges.size(); ++i) {
        size_t other = vertices[root].in_edges[i]->from_id;
        shuffled_edges.push_back(other);
        
        assert(other != root);
    }
    
    random_shuffle(shuffled_edges.begin(), shuffled_edges.end());
    
    for(int i = 0; i < shuffled_edges.size(); ++i) {
        size_t neighbor = shuffled_edges[i];
        bfs_queue.push(neighbor);
        visited.insert(neighbor);
    }
    
    //cout << splash_work << endl;
    
    while(splash_work < splash_size && !bfs_queue.empty()) {
        size_t vertex = bfs_queue.front();
        bfs_queue.pop();
        size_t vertex_work = work(vertex);
        
        if(vertex_work + splash_work > splash_size) continue;
        
        size_heap& pqueue(pqueues[vmap[vertex]]);
        bool success = pqueue.remove(vertex);
        
        if(!success) { continue; }
        
        //cout << vertex_work << endl;
        splash.push_back(vertex);
        splash_work += vertex_work;
        
        shuffled_edges.clear();
        vector<edge_data*>& in_edges(vertices[vertex].in_edges);
        
         //assert(vertices[vertex].neighbors.size() == vertices[vertex].in_edges.size());
         
        for(int i = 0; i < in_edges.size();
                ++i) {
            shuffled_edges.push_back(in_edges[i]->from_id); 
            //shuffled_edges.push_back(vertices[vertex].neighbors[i]); // XXX
        }
        
        random_shuffle(shuffled_edges.begin(), shuffled_edges.end());
        
        for(int i = 0; i < shuffled_edges.size(); ++i) {
            size_t neighbor = shuffled_edges[i];
            
            if(visited.count(neighbor) == 0) {
                visited.insert(neighbor);
                bfs_queue.push(neighbor);
            }
        }
    }
    
    size_t original_size = splash.size();
    if(original_size > 1) {
        reverse(splash.begin(), splash.end());
        splash.resize(original_size * 2 - 1);
        for(size_t i = 0; i < original_size - 1; ++i) {
            splash.at(splash.size() - 1 - i) = splash.at(i);
        }
    }
    
    //cout << splash.size() << endl;
}

static inline void
init_engine(void)
{
    rebuild_splash(0);
}

static inline bool
active_set_clear_bit(int vertex)
{
    bool old = active_set[vertex];
    
    active_set[vertex] = false;
    
    return old;
}

static void
run_engine(void)
{
    size_t cpuid = 0;
    
    assert(cpuid < splashes.size());
    while(true) {
        if((splash_index[cpuid] >= splashes[cpuid].size()))
            rebuild_splash(cpuid);
        
        if(splash_index[cpuid] >= splashes[cpuid].size()) return;
        
        while(splash_index[cpuid] < splashes[cpuid].size()) {
            size_t vertex = splashes[cpuid][splash_index[cpuid]++];
            size_heap& pqueue(pqueues[vmap[vertex]]);
            
            pqueue.remove(vertex);
            
            if(active_set_clear_bit(vertex)) {
                update_count++;
                bp_update(vertices[vertex]);
            }
        }
    }
}
#else
static list<vertex_data*> fifo;

static void
add_task(vertex_data &vdata, const double priority)
{
    if(vdata.in_queue)
        return;
    fifo.push_back(&vdata);
    vdata.in_queue = true;
}

static void
add_all_tasks_to_fifo(const double priority)
{
    for(vector<vertex_data>::iterator it(vertices.begin()), end(vertices.end());
            it != end;
            ++it)
    {
        add_task(*it, priority);
    }
}

static void
start_engine(void)
{
}

static inline void
init_engine(void)
{
    
}

static void
run_engine(void)
{
    while(!fifo.empty()) {
        vertex_data *vdata(fifo.front());
        fifo.pop_front();
        vdata->in_queue = false;
        update_count++;
        if(update_count % 10 == 0)
           readMemory();
        bp_update(*vdata);
    }
}
#endif

int main(int argc, char** argv)
{
   initMemory();
#if 0
    size_heap h;
    
    h.push(2, 2.0);
    h.push(4, 4.0);
    h.push(3, 3.0);
    h.push(10, 10.0);
    h.push(1, 1.0);
    
    //h.print();
    
    pair<size_t, double> d;
    
    while(!h.empty()) {
        d = h.pop();
        cout << d.first << " " << d.second << endl;
    }
    
    return 0;
#endif
    
    if(argc == 2) {
       GRID_SIZE = (size_t)atoi(argv[1]);
    }
#ifndef MEASURE_MEM
    cout << "Create image " << GRID_SIZE << "x" << GRID_SIZE << "\n";
#endif
    set_size(GRID_SIZE, GRID_SIZE);
    paint_sunset(5);
#ifdef OUTPUT_IMAGES
    save_image("original.pgm");
#endif
#ifndef MEASURE_MEM
    cout << "Corrupt image.\n";
#endif
    corrupt_image(sigma);
#ifdef OUTPUT_IMAGES
    save_image("corrupted.pgm");
#endif
    
#ifndef MEASURE_MEM
    cout << "Construct graph.\n";
#endif
    construct_graph();
    
    assert(colors > 0);
    edge_potential = new binary_factor(0, colors, 0, colors);
    edge_potential->set_as_laplace(lambda);
#ifdef OUTPUT_DATA
    //edge_potential->output_as("global-potential");
    print_data();
#endif
    
    assert(vertices.size() == rows * cols);
    
#ifndef OUTPUT_DATA
    start_engine();
    readMemory();
    add_all_tasks_to_fifo(100.0);
    readMemory();
    init_engine();
    readMemory();
    
#ifndef MEASURE_MEM
    cout << "Running engine.\n";
#endif
    run_engine();
    
#ifndef MEASURE_MEM
    cout << "Run everything with " << update_count << " updates" << endl;
#endif
    
#ifdef OUTPUT_IMAGES
    reconstruct_image();
    
    save_image("fixed.pgm");
#endif
#ifdef OUTPUT_RESULTS
    output_results();
#endif
#endif
    delete_graph();
    printMemory();
    
    return 0;
}

