from typing import List, Dict, Optional
import sys
import os
import json
import re


class CodeLine:
    def __init__(self, data: str, file_name: str, start: int, length: int, bug_id: Optional[int]):
        self.data = data
        self.file_name = file_name
        self.start = start
        self.length = length
        self.bug_id = bug_id

    def is_bug(self):
        return self.bug_id is not None


class SourceCode:
    bench_pattern = re.compile(r"#ifdef RACEBENCH_BUG_(\d+)")

    def __init__(self, path: str):
        self.path = path
        with open(path) as f:
            lines = f.readlines()
        self.codes: List[CodeLine] = []
        i = 0
        while i < len(lines):
            line = lines[i]
            match = SourceCode.bench_pattern.search(line)
            if match is None:
                if line.startswith("#include \"racebench_bugs.h\""):
                    code = CodeLine(line, self.path, i + 1, 1, -1)
                else:
                    code = CodeLine(line, self.path, i + 1, 1, None)
                i += 1
            else:
                bug_id = int(match.group(1))
                data = line
                for j in range(i + 1, len(lines)):
                    data += lines[j]
                    if "#endif" in lines[j]:
                        break
                code = CodeLine(data, self.path, i + 1, j - i + 1, bug_id)
                i = j + 1
            self.codes.append(code)

    def get_code_line(self, line_num: int, bug_id: int) -> Optional[CodeLine]:
        cur_line_num = 1
        for code in self.codes:
            if not code.is_bug():
                cur_line_num += code.length
                continue
            if code.bug_id == bug_id:
                if cur_line_num == line_num:
                    return code
        return None


class Target:
    def __init__(self, target_root: str):
        self.root = target_root
        self.code_dir = os.path.join(self.root, "code")
        self.trace_dir = os.path.join(self.root, "trace")
        self.code_cache: Dict[str, SourceCode] = dict()

    def _get_code_cached(self, path: str) -> SourceCode:
        if path in self.code_cache:
            return self.code_cache[path]
        src = SourceCode(os.path.join(self.code_dir, path))
        self.code_cache[path] = src
        return src

    def _get_code_line(self, path: str, line_num: int, bug_id: int) -> CodeLine:
        code = self._get_code_cached(path)
        code_line = code.get_code_line(line_num, bug_id)
        assert code_line is not None
        return code_line

    def get_races(self, bug_id: int) -> List[List[CodeLine]]:
        info_path = os.path.join(self.trace_dir, "bug-%d.json" % bug_id)
        with open(info_path) as f:
            bug_info = json.load(f)
        races = []
        for item in bug_info:
            if item["type"] != "pattern":
                continue
            locations = item["locations"]
            cur_race = []
            for loc in locations:
                code_line = self._get_code_line(loc["file"], loc["line"], bug_id)
                cur_race.append(code_line)
            races.append(cur_race)
        return races


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: %s <target> <bug_id>" % sys.argv[0])
        exit(1)
    target_root = sys.argv[1]
    bug_id = int(sys.argv[2])
    target = Target(target_root)
    races = target.get_races(bug_id)
    for race in races:
        race = [(code.file_name, code.start, code.length) for code in race]
        print(race)
