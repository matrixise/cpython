import sys
import argparse
import importlib
from asyncio.runners import run

DESCRIPTION = """
A simple command line interface for the asyncio module.

python -m asyncio module_name:coroutine
"""

def parse_args():
    parser = argparse.ArgumentParser(
        prog='asyncio',
        description=DESCRIPTION,
        add_help=True,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument('--debug', action='store_true',
        help='if enabled, the event loop will be run in debug mode')
    parser.add_argument("coroutine_path", nargs=1)
    return parser.parse_args()


def main():
    args = parse_args()
    from pprint import pprint as pp
    pp(args)

    module_name, coroutine_name = args.coroutine_path[0].split(":")
    module = importlib.import_module(module_name)
    coroutine = getattr(module, coroutine_name)
    run(coroutine(), debug=args.debug)


if __name__ == "__main__":
    main()
