#include "ContentGenerators.h"
#include "log.h"
#include "util.h"

#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception_ptr.hpp>
#include <boost/json/src.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

namespace dummy_data_generator {

const char * kDefaultVal    = "DEFAULT-VALUE";
const size_t kInitNodeCount = 1000; // count of first pick nodes
const double kDecayRatio    = 0.8;  // 0.8 ^ 10 ==> 0.1
const size_t kDegreeMin     = 3;    // normal distribution
const size_t kDegreeMax     = 10;   //

struct PropDesc {
    std::string                                    name;
    std::string                                    desc;
    std::shared_ptr<tusion::gen::ContentGenerator> cgr;
    PropDesc(const std::string &pname, const std::string &pval)
        : name(pname), desc(pval), cgr(tusion::gen::GetContentGenerator(pval)) {
    }

    std::string GetContent(const std::string &s = kDefaultVal) {
        std::string v;
        if (cgr.get() != nullptr) {
            cgr->serilize(v);
            return v;
        } else {
            return s;
        }
    }
    bool IsOk() { return cgr.get() != nullptr; }

    std::string ToString() {
        return fmt::format("n={}, d={}, gen={}", name, desc,
                           cgr.get() != nullptr);
    }
};

struct ControlBlock {
    size_t      count;
    size_t      depth_min;
    size_t      depth_max;
    size_t      default_depth;
    std::string src_tag_name;
    std::string dst_tag_name;

    std::shared_ptr<tusion::gen::GeneratorRange> dg;

    ControlBlock(boost::json::value &ctx)
        : count(0), depth_min(0), depth_max(0), src_tag_name(""),
          dst_tag_name("") {
        try {
            count = ctx.at("count").as_int64();
        } catch (...) {
        }

        try {
            std::string drangestr = ctx.at("depth").as_string().data();
            if (tusion::gen::GetRange(drangestr, dg)) {
                depth_min = dg->min;
                depth_max = dg->max;
            }
        } catch (...) {
        }

        try {
            src_tag_name = ctx.at("srcname").as_string().data();
        } catch (...) {
        }

        try {
            dst_tag_name = ctx.at("dstname").as_string().data();
        } catch (...) {
        }

        default_depth = (depth_min + depth_max) / 2;
        default_depth = default_depth == 0 ? 3 : default_depth;
    }
    std::string ToString() {
        return fmt::format("cnt={},depth=[{},{}],def_depth={},src={},dst={}",
                           count, depth_min, depth_max, default_depth,
                           src_tag_name, dst_tag_name);
    }

    size_t GetDepth() {
        size_t v = default_depth;
        if (dg) {
            dg->generate(v);
        }
        return v;
    }
};
struct CustomGeneratorConCat : public tusion::gen::ContentGenerator {
    std::shared_ptr<tusion::gen::GeneratorString> gen1;
    std::shared_ptr<tusion::gen::GeneratorString> gen2;
    CustomGeneratorConCat()
        : gen1(new tusion::gen::GeneratorString(20)),
          gen2(new tusion::gen::GeneratorString(14)) {}
    virtual void serilize(std::string &content) {
        std::string v1, v2;
        gen1->serilize(v1);
        gen2->serilize(v2);
        content = fmt::format("{}_{}", v1, v2);
    }
};

struct TagDesc {
    std::string name;

    std::string                                    viddesc;
    std::shared_ptr<tusion::gen::ContentGenerator> vidgen;
    std::shared_ptr<ControlBlock>                  cb;

    std::vector<PropDesc> props;

    std::string ToString() {
        std::string out = "{\n+ TagSumm:\n";
        auto        a   = fmt::format("n={}, vid={}, vidg={}", name, viddesc,
                             vidgen.get() != nullptr);
        auto        b   = cb->ToString();
        out += a;
        out += "\nControlBlock: \n";
        out += b;
        out += "\nProperties: \n";
        for (auto &i : props) {
            out += i.ToString();
            out += "\n";
        }
        out += "}\n";
        return out;
    }

    // runtime
    std::vector<std::string> vlist;

    TagDesc(boost::json::value &ctx) {
        try {
            name    = ctx.at("name").as_string().data();
            viddesc = ctx.at("vid").as_string().data();
            if (viddesc.compare("@string(20)_string(14)") == 0) {
                vidgen.reset(new CustomGeneratorConCat);
            } else {
                vidgen = tusion::gen::GetContentGenerator(viddesc);
            }

            auto cbjv = ctx.at("control");
            cb.reset(new ControlBlock(cbjv));

            auto p = ctx.at("props").as_object();
            props.reserve(p.size());
            for (auto pit = p.begin(); pit != p.end(); pit++) {
                std::string propname(pit->key().data(), pit->key().size());
                std::string propval(pit->value().as_string().data());

                props.emplace_back(propname, propval);
            }
        } catch (...) {
        }
    }
};

struct EdgeDesc {
    std::string                   name;
    std::shared_ptr<ControlBlock> cb;
    std::vector<PropDesc>         props;

    // runtime
    std::shared_ptr<TagDesc> from;
    std::shared_ptr<TagDesc> to;
    struct edge {
        size_t srcIdx;
        size_t dstIdx;
        edge(size_t s, size_t d) : srcIdx(s), dstIdx(d) {}
    };
    static const int constexpr kEdgeBufferSize =
        1000; // each generation thread maintain it's edge buffer
    // std::shared_ptr<std::vector<struct edge>> edge_buffer;

    EdgeDesc(boost::json::value &ctx) {
        try {
            name = ctx.at("name").as_string().data();

            auto cbjv = ctx.at("control");
            cb.reset(new ControlBlock(cbjv));

            auto p = ctx.at("props").as_object();
            props.reserve(p.size());
            for (auto pit = p.begin(); pit != p.end(); pit++) {
                std::string propname(pit->key().data(), pit->key().size());
                std::string propval(pit->value().as_string().data());

                props.emplace_back(propname, propval);
            }
        } catch (...) {
        }
    }

    std::string ToString() {
        std::string out = "{\n+ EdgeSumm: \n";
        out += name;
        auto b = cb->ToString();
        out += "\n+ ControlBlock: \n";
        out += b;
        out += "\n+ Properties: \n";
        for (auto &i : props) {
            out += i.ToString();
            out += "\n";
        }
        out += "}\n";
        return out;
    }
};

std::string GetConfText(const std::string &fpath = "sample.json") {
    std::ifstream      ifs{fpath};
    std::ostringstream sstr;
    sstr << ifs.rdbuf();
    return sstr.str();
}

std::unordered_map<std::string, std::shared_ptr<TagDesc>>  gTagMap;
std::unordered_map<std::string, std::shared_ptr<EdgeDesc>> gEdgeMap;

void Show() {
    log_info("====== Show ======");
    for (auto &iv : gTagMap) {
        log_info("tag: {} ==> ", iv.first);
        log_info("\n{}", iv.second->ToString());
    }
    for (auto &iv : gEdgeMap) {
        log_info("edge: {} ==> ", iv.first);
        log_info("\n{}", iv.second->ToString());
    }
    log_info("============");
}

bool ReadConfig(const std::string &filepath) {
    std::string conftext = GetConfText(filepath);

    boost::json::error_code    ec;
    boost::json::parse_options opt;
    opt.allow_comments = true;

    auto jv = boost::json::parse(conftext, ec, boost::json::storage_ptr(), opt);

    if (ec) {
        log_info("read config err: {}, \n---conf text---\n{} ---\n",
                 ec.message(), conftext);
        return false;
    }
    auto ctxs = jv.get_array();
    for (size_t i = 0; i < ctxs.size(); i++) {
        auto        ctx   = ctxs.at(i);
        std::string ttype = ctx.at("type").as_string().data();
        std::string tname = ctx.at("name").as_string().data();
        log_info("--- t:{}, n:{} --- ", ttype, tname);

        if (ttype.compare("tag") == 0) {
            std::shared_ptr<TagDesc> ptrTd(new TagDesc(ctx));
            gTagMap[tname] = ptrTd;
        } else if (ttype.compare("edge") == 0) {
            std::shared_ptr<EdgeDesc> ptrEd(new EdgeDesc(ctx));
            gEdgeMap[tname] = ptrEd;
        }
    }
    return true;
}

void TagGenerationVid(std::shared_ptr<TagDesc> td, size_t count) {
    log_info("Tag {}: generating vid ...", td->name);

    td->vlist.reserve(count);
    for (size_t i = 0; i < count; i++) {
        std::string ctn;
        td->vidgen->serilize(ctn);
        td->vlist.emplace_back(std::move(ctn));
    }

    log_info("Tag {}: vid gen done, count: {}", td->name, count);
}

// step 1. VID generation
void GenerateVids(size_t cnt) {
    std::vector<std::thread> threads;
    for (auto &iv : gTagMap) {
        log_info("adding thread to generate vid ...");
        threads.push_back(std::thread(TagGenerationVid, iv.second, cnt));
    }

    for (auto &th : threads) {
        th.join();
    }
    log_info("vid generation done...");
}

void MakeEdges(std::shared_ptr<EdgeDesc> ed, size_t degreeMin, size_t degreeMax,
               size_t initPick, double decayRatio, size_t count) {
    size_t                      ldepth, lastPick = initPick;
    size_t                      gcount = 0;
    tusion::gen::GeneratorRange degreepicker(degreeMin, degreeMax);

    log_info("make edge: {}, degree: [{},{}], initpick: {}, decay: {}, count: {}",
            ed->name, degreeMin, degreeMax, initPick, decayRatio, count);

    std::shared_ptr<std::vector<EdgeDesc::edge>> edge_buffer;

    auto nodePicker = [](size_t cnt, size_t min_, size_t max_,
                         std::vector<size_t> &result,
                         std::vector<size_t> *src = nullptr,
                         bool conv = false) {
        tusion::gen::GeneratorRange p(min_, max_);
        result.clear();
        while (cnt > 0) {
            size_t pick;
            p.generate(pick);
            if (src != nullptr && conv) {
                pick = (*src)[pick];
            }
            result.push_back(pick);
            cnt--;
        }
    };
    auto consumeEdge = [&edge_buffer, &ed] () {
        log_info("consume {} edges ...", edge_buffer->size());
        for (auto & eg : *edge_buffer) {
            log_info("edge: [{}]:{} -> [{}]:{}",
                    ed->from->name, ed->from->vlist[eg.srcIdx],
                    ed->to->name, ed->to->vlist[eg.dstIdx]);
        }
    };

    auto collectEdge = [&ed, &edge_buffer, &gcount, &consumeEdge](size_t s, size_t d) {
        if (edge_buffer.get() == nullptr) {
            edge_buffer.reset(new std::vector<EdgeDesc::edge>);
        }
        edge_buffer->emplace_back(s, d);
        gcount++;

        if (edge_buffer->size() > EdgeDesc::kEdgeBufferSize) {
            // consume to output thread.
            consumeEdge();

            // output & realloc new buffer....
            edge_buffer.reset(new std::vector<EdgeDesc::edge>);
        }
    };

    auto showvec = [](const std::string & title, std::vector<size_t> & v) {
        log_info("==== {} =====", title);
        for (auto & i : v) {
            log_info("{}", i);
        }
    };

    auto generateEdge = [&] (std::vector<size_t> *srcfromset, std::vector<size_t> *dstfromset,
            std::vector<size_t> *srcnodes, std::vector<size_t> *dstnodes, bool conv) {
        srcnodes->reserve(initPick * degreeMax);
        dstnodes->reserve(initPick * degreeMax);
        srcnodes->clear();
        dstnodes->clear();

        std::vector<size_t> sn_dsts;
        sn_dsts.reserve(degreeMax);

        log_info("pick {} nodes index range[{},{}] as src point ...", lastPick, 0, srcfromset->size() - 1);
        nodePicker(lastPick, 0, srcfromset->size() - 1, (*srcnodes), srcfromset, conv);
        showvec("pick src:", *srcnodes);

        for (auto &sn : (*srcnodes)) {
            size_t d;
            degreepicker.generate(d);
            std::stringstream ss;
            ss << sn << " --> degree: " << d << " --> [";
            nodePicker(d, 0, dstfromset->size() - 1, sn_dsts, dstfromset, conv);
            for (auto &dn : sn_dsts) {
                collectEdge(sn, dn);
                ss << dn << ",";
            }
            ss << "]";
            log_info("{}", ss.str());
            dstnodes->insert(dstnodes->end(), sn_dsts.begin(), sn_dsts.end());
        }

        lastPick = lastPick * decayRatio;

        showvec("pick dst:", *dstnodes);
    };
    std::vector<size_t> fakelist_from, fakelist_to;
    fakelist_from.resize(ed->from->vlist.size());
    fakelist_to.resize(ed->to->vlist.size());

    log_info("generating edges for {} ...", ed->name);
    while (gcount < count && lastPick > 0) {
        std::vector<size_t> src_nodes[2], dst_nodes[2];
        size_t src_idx = 0, dst_idx = 0;
        std::vector<size_t> *src_from, *src_to, *dst_from, *dst_to;

        src_from = &fakelist_from;
        dst_from = &fakelist_to;
        src_to = &(src_nodes[src_idx]);
        dst_to = &(dst_nodes[dst_idx]);

        /* first batch pick */
        generateEdge(
                src_from,
                dst_from,
                src_to /*source results*/,
                dst_to /*dest   results*/,
                false);

        log_info("-----first round pick, has gen {} edges ----", gcount);

        ed->cb->dg->generate(ldepth);

        src_idx++;
        src_idx = src_idx % 2;
        dst_idx++;
        dst_idx = dst_idx % 2;

        size_t dep = ldepth;
        log_info("ldepth {}.", ldepth);

        while (ldepth > 1 && lastPick > 0 && gcount < count) {
            /* more depth batch pick if needed */
            src_from = dst_to;
            dst_from = src_to;
            src_to = &(src_nodes[src_idx]);
            dst_to = &(dst_nodes[dst_idx]);
            generateEdge(
                src_from,
                dst_from,
                src_to /*source results*/,
                dst_to /*dest   results*/,
                true);

            log_info("-----{} round pick, src pick count: {}, has gen {} edges ----", dep - ldepth, lastPick, gcount);
            src_idx++;
            src_idx = src_idx % 2;
            dst_idx++;
            dst_idx = dst_idx % 2;
            ldepth--;
        }
    }

    consumeEdge(); // consume at last
}

// step 2. path with depth generation
bool EdgeGeneration(std::shared_ptr<EdgeDesc> ed, size_t count) {
    // from tag & to tag
    std::string from_tag_name = ed->cb->src_tag_name,
                to_tag_name   = ed->cb->dst_tag_name;
    auto from                 = gTagMap.find(from_tag_name);
    auto to                   = gTagMap.find(to_tag_name);
    if (from == gTagMap.end() || to == gTagMap.end()) {
        log_info("edge's tag not found: {}, {}", from_tag_name, to_tag_name);
        return false;
    }
    ed->from = from->second;
    ed->to   = to->second;
    MakeEdges(ed, /*kDegreeMin, kDegreeMax, kInitNodeCount, kDecayRatio*/
            1, 3, 5, 0.5,
            count);
}

bool EdgeGenerationFull(size_t count) {
    for (auto &iv : gEdgeMap) {
        bool res = EdgeGeneration(iv.second, count);
    }
}

void TestConfigParse() {
    std::string conftext = GetConfText();

    boost::json::error_code    ec;
    boost::json::parse_options opt;
    opt.allow_comments = true;

    auto jv = boost::json::parse(conftext, ec, boost::json::storage_ptr(), opt);

    auto ctxs = jv.get_array();
    for (size_t i = 0; i < ctxs.size(); i++) {
        auto        ctx   = ctxs.at(i);
        std::string ttype = ctx.at("type").as_string().data();
        std::string tname = ctx.at("name").as_string().data();
        log_info("--- t:{}, n:{} --- ", ttype, tname);

        if (ttype.compare("tag") == 0) {
            TagDesc td(ctx);
            log_info("tag:========\n{}=======\n", td.ToString());
        } else if (ttype.compare("edge") == 0) {
        }
    }
}

void TestVidGen() {
    log_info("Reading Configs ....");
    ReadConfig("sample.json");
    log_info("Reading Configs Done ....");
    Show();

    GenerateVids(10);

    for (auto &iv : gTagMap) {
        log_info("tag: {} ...", iv.second->name);
        size_t ii = 0;
        for (auto &iy : iv.second->vlist ) {
            log_info("[{:02d}] : {}",  ii++, iy);
        }
    }

    EdgeGenerationFull(30);
}

} // namespace dummy_data_generator

int main(int argc, char **argv) {
    tusion::util::init_module_console_log();
    /*
    log_info("Reading Configs ....");
    dummy_data_generator::ReadConfig("sample.json");
    log_info("Reading Configs Done ....");
    dummy_data_generator::Show();
    */

    dummy_data_generator::TestVidGen();
    return 0;
}

