

#include <cstring>

#include "RTTL/common/MapOptions.hxx"

namespace RTTL {

MapOptions options;

static _INLINE char *trimEnd(char *token) {
    char *pe = token + strlen(token) - 1;
    while ((*pe == ' ' || *pe == '\t' || *pe == 10 || *pe == 13) && pe >= token) {
        *pe-- = 0;
    }
    return token;
}

static _INLINE int pathLength(const string &fn) {
#ifdef _WIN32
    const char *del = "\\/:";
#else
    const char *del = "/";
#endif
    return string(fn).find_last_of(del) + 1;
}

MapOptions::~MapOptions() {

    bool start = true;
    for (iterator it = begin(); it != end(); it++) {
        vector_of_strings *entry = it->second;
        map<void *, bool>::iterator itused = m_used_entries.find(it->second);
        if (itused->second == false) {
            if (start) {
                start = false;
                puts("\n");
            }
        }
    }

    m_used_entries.clear();

    clear();
}

void MapOptions::remove_all_entries() {
    for (iterator it = begin(); it != end(); it++) {
        vector_of_strings *entry = it->second;
        entry->clear();
        delete entry;
    }
}

void MapOptions::clear() {
    remove_all_entries();
    map_strings_to_vector_of_strings::clear();
}

void MapOptions::add_string(const string &name, const string &value) {
    (*this)[name]->push_back(value);
}

string MapOptions::get(string name) const {
    return get(name, name, 0);
}

RTVec3f MapOptions::getVec3f(string name, float defvalue) const {
    return RTVec3f(getVector<3, float>(name, defvalue));
}

RTVec3f MapOptions::getVec3i(string name, int defvalue) const {
    return RTVec3f(getVector<3, int>(name, defvalue));
}

RTVec2f MapOptions::getVec2f(string name, float defvalue) const {
    return RTVec2f(getVector<2, float>(name, defvalue));
}

RTVec2i MapOptions::getVec2i(string name, int defvalue) const {
    if (defined(name)) {
        string rs = get(name);
        const char *rc = rs.c_str();
        int lx = strcspn(rc, "xX");
        if (rc[lx]) {
            return RTVec2i(atoi(rc), atoi(rc + lx + 1));
        } else {
            return getVector<2, int>(name, defvalue);
        }
    }
    return RTVec2i(defvalue);
}

int MapOptions::verbose() const {
    const_iterator it = find("verbose");
    if (it == end()) {
        return 0;
    }
    const vector_of_strings &entry = *it->second;
    return Convert::to<int>(entry[0]);
}

unsigned int MapOptions::vector_size(const string &name) const {
    const_iterator it = find(name);
    if (it == end()) {
        return 0;
    } else {
        vector_of_strings &entry = *it->second;
        return entry.size();
    }
}

bool MapOptions::defined(const char *name, int len) const {
    return defined(string(name, len));
}

bool MapOptions::defined(const string &name) const {
    return find(name) != end();
}

void MapOptions::remove(const string &name) {

    iterator it = find(name);
    if (it != end()) {
        erase(it);
    }
}

bool MapOptions::parse(int argc, char *argv[], bool keep_all_filenames) {

    return parse(argc, (const char **)argv, keep_all_filenames);
}

bool MapOptions::parse(const char *a) {
    const char *argv[] = {a};
    return parse(1, argv);
}

bool MapOptions::isNumber(const char *s) {

    int nss = 0, ndd = 0, nee = 0;
    int n = 0;
    do {
        int ns = s[n] == '-' || s[n] == '+';

        if (ns && n && s[n - 1] != 'e' && s[n - 1] != 'E') {
            return false;
        }

        if (ns && !(s[n + 1] >= '0' && s[n + 1] <= '9')) {
            return false;
        }
        nss += ns;
        int nd = s[n] == '.';

        if (nd && !(s[n + 1] == 0 || s[n + 1] == 'e' || s[n + 1] == 'E' || (s[n + 1] >= '0' && s[n + 1] <= '9'))) {
            return false;
        }

        if (nd && nee) {
            return false;
        }
        ndd += nd;

        if (nss > 2 || ndd > 1) {
            return false;
        }
        int ne = s[n] == 'e' || s[n] == 'E';
        nee += ne;

        if (nee > 2 || (ne && !n)) {
            return false;
        }
        if (ne) {
            n++;
            if (s[n] == 0 || strchr(" \t,;])", s[n])) {
                return false;
            }
            continue;
        }
        if (!((s[n] >= '0' && s[n] <= '9') || ns || nd)) {
            return false;
        }
        n++;
    } while (s[n] && !strchr(" \t,;])", s[n]));
    return true;
}

bool MapOptions::parse(int argc, const char *argv[], bool keep_all_filenames) {
    const char *name = 0;
    const char *ptr;
    int added = 1;
    int namelen;
    for (int count = 0; count < argc; count++) {

        const char *arg = argv[count];
        if (isNumber(arg)) {
            if (!name) {
                printf("Unnamed number: %s\n", arg);
                return false;
            }
            goto parse_parameters;
        }

        if (arg[0] == '-' || arg[0] == '+') {

            if (name && !added) {

                add(name, "");
            }

            bool accumulate = arg[0] == '+';

            name = arg + 1;
            while (*name == '-') {
                name++;
            }
            added = 0;
            namelen = strcspn(name, "=");

            if (!accumulate) {

                remove(string(name, namelen));
            }

            if (namelen == strlen(name)) {
                continue;
            }

            arg = name + namelen + 1;
            arg += strspn(arg, " \t");
            goto parse_parameters;

        } else if (added && (ptr = strrchr(arg, '.'))) {

#ifdef _WIN32

            char *parg = (char *)arg;
            while (parg = strchr(parg, '/'))
                *parg = '\\';
#endif
            if (keep_all_filenames == false && access(arg, 0) == -1) {
                printf("File %s does not exist.\n", arg);
                return false;
            }

            if (strcasecmp(ptr + 1, "ini") == 0) {
                if (parse_file(arg) == false) {
                    return false;
                }
            } else {

                add("files", arg);
            }
            name = 0;

        } else if (name) {
        parse_parameters:

            char term[PATH_MAX];

            bool remove_separator = *arg == '=' || *arg == '(' || *arg == '[' || *arg == ',' || *arg == ';';
            strncpy(term, arg + (remove_separator ? 1 : 0), sizeof(term));
            int remain = strlen(term);
            char *pterm = term;

        unfuse:
            int term_end = strcspn(pterm, ",;])");
            pterm[term_end] = 0;

            if (*pterm == 0) {
                continue;
            }

            ++added;
            add(string(name, namelen), pterm);
            pterm += term_end + 1;
            remain -= term_end + 1;
            if (remain > 0) {
                goto unfuse;
            }

        } else {
            return false;
        }
    }
    if (name && !added) {

        add(name, "");
    }

    return true;
}

bool MapOptions::parse_file(const char *filename) {
    FILE *fs = fopen(filename, "rt");
    if (fs == 0) {
        printf("File %s does not exist.\n", filename);
        return false;
    }

    char buf[300];
    char prefix[80];
    prefix[0] = 0;

    while (fgets(buf, sizeof(buf), fs)) {
        trimEnd(buf);

        char *pname = buf + strspn(buf, " \t\n");
        if (*pname == 0 || (pname[0] == '/' && pname[1] == '/')) {
            continue;
        }

        if (*pname == '#') {
            continue;
        }

        if (!strncmp(pname, "exit", 4)) {
            break;
        }

        if (*pname == '[') {

            strncpy(prefix, pname + 1, sizeof(prefix));
            int prefixend = strcspn(prefix, "] \t");
            prefix[prefixend] = 0;
            continue;
        }

        if (pname[0] == '-' || pname[0] == '+') {

            trimEnd(pname);

        split_multiple_tokens_into_groups:
            int nameend = strcspn(pname, " \t");
            int namelen = strlen(pname);
            if (nameend < namelen) {
                pname[nameend] = '=';
            }

            char *nextpname = pname;
        next_token:
            unsigned int postoken = strcspn(++nextpname, "-+");
            if (postoken < strlen(nextpname)) {
                nextpname += postoken;
                if (nextpname[-1] == ' ' || nextpname[-1] == '\t') {
                    if (nextpname[1] >= '0' && nextpname[1] <= '9') {

                        goto next_token;
                    } else {

                        nextpname[-1] = 0;
                    }
                } else {
                    goto next_token;
                }
            } else {
                nextpname = NULL;
            }
            parse(pname);
            if (!nextpname) {
                continue;
            }
            pname = nextpname;
            goto split_multiple_tokens_into_groups;

        } else if (strrchr(pname, '.') && !strrchr(pname, '=')) {

            if (access(pname, 0) == -1) {

                const string fnini(filename);
                const string fullfn = fnini.substr(0, pathLength(fnini)) + string(pname);
                const char *pname = fullfn.c_str();
                if (access(pname, 0) != -1) {

                    parse(pname);
                } else {

                    FATAL(string("Cannot open file ") + string(pname));
                }
            } else {

                parse(pname);
            }
            continue;
        }

        bool accumulate = *pname == '+';
        if (accumulate) {
            pname++;
        }

        int name_end = strcspn(pname, "= \t");
        pname[name_end] = 0;

        char *pvalue = pname + name_end + 1;

        bool first_token = true;
        string token_name = *prefix ? string(prefix) + string(".") + string(pname) : string(pname);
        if (!accumulate) {

            remove(token_name);
        }

    next_value:
        pvalue += strspn(pvalue, "=,;[( \t\n");
        if (*pvalue == 0) {
            continue;
        }

        int value_end;
        if (*pvalue == '\"') {

            value_end = strcspn(++pvalue, "\"\n");
        } else {

            value_end = strcspn(pvalue, ",;]) \t\n");
        }
        pvalue[value_end] = 0;

        if (first_token) {
            if (strcasecmp(pname, "include") == 0 || strcasecmp(pname, "#include") == 0) {

                if (!parse_file(pvalue)) {
                    return false;
                }
                continue;
            }
            if (*pname == '#') {
                continue;
            }
            first_token = false;
        }

        add(token_name, pvalue);

        pvalue += value_end + 1;
        goto next_value;
    }

    fclose(fs);
    return true;
}

const vector_of_strings *MapOptions::operator[](const string &name) const {
    const_iterator it = find(name);
    if (it == end()) {
        return 0;
    }
    return it->second;
}

vector_of_strings *MapOptions::operator[](const string &name) {
    vector_of_strings *entry;
    iterator it = find(name);
    if (it == end()) {
        entry = new vector_of_strings;
        insert(value_type(name, entry));
        m_used_entries.insert(map<void *, bool>::value_type(entry, false));
    } else {
        entry = it->second;
    }
    return entry;
}

MapOptions::iterator MapOptions::find(const string &name) {
    iterator it;
    const char *ns = name.c_str();
    const char *del = ",; \t|";
    if (strcspn(ns, del) == strlen(ns)) {
        it = map_strings_to_vector_of_strings::find(name);
    } else {

        const char *pname = ns;
        do {
            int sep = strcspn(pname, del);
            it = find(pname, sep);
            if (it != end()) {
                break;
            }
            pname += sep;
            pname += strspn(pname, del);
        } while (*pname && *pname != '\n');
    }

    if (it != end()) {
        map<void *, bool>::iterator itused = m_used_entries.find(it->second);
        assert(itused != m_used_entries.end());
        itused->second = true;
    }
    return it;
}
MapOptions::const_iterator MapOptions::find(const string &name) const {

    return (const_iterator)((MapOptions *)this)->find(name);
}
MapOptions::iterator MapOptions::find(const char *name, int len) {
    return find(string(name, len));
}
MapOptions::const_iterator MapOptions::find(const char *name, int len) const {
    return (const_iterator)find(string(name, len));
}
}; // namespace RTTL
