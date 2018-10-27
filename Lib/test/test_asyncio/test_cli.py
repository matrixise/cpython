import os
import unittest

from test import support
from test.support.script_helper import (
    assert_python_ok,
    assert_python_failure
)

TEMPDIR = os.path.abspath(support.TESTFN) + '-asyncio'

TEST_ASYNCIO = """\
async def amain():
    print("hello")
"""

class TestAsyncIO_CLI(unittest.TestCase):
    def get_args(self, *args):
        return ('-m', 'asyncio') + args
    def test_help(self):
        rc, out, err = assert_python_ok(*self.get_args('-h'))
        self.assertIn(b'A simple command line', out)

    def test_split_coroutine_path(self):
        args = self.get_args('test_asyncio:amain:hello')
        rc, out, err = assert_python_failure(*args)
        
        self.assertIn(
            b'ValueError: too many values to unpack (expected 2)',
            err
        )

    def test_empty_module_name(self):
        args = self.get_args(':amain')
        rc, out, err = assert_python_failure(*args)

        self.assertIn(b'ValueError: Empty module name', err)

    def test_no_coroutine_name(self):
        args = self.get_args('amain')
        rc, out, err = assert_python_failure(*args)

        self.assertIn(
            b'ValueError: not enough values to unpack (expected 2, got 1',
            err
        )

    def test_module_does_not_exist(self):
        args = self.get_args('demo_module:coroutine')
        rc, out, err = assert_python_failure(*args)

        self.assertIn(
            b"ModuleNotFoundError: No module named 'demo_module'",
            err
        )

    def test_coroutine_does_not_exist(self):
        args = self.get_args('demo_module:coroutine')
        try:
            os.makedirs(TEMPDIR)
            demo_module_path = os.path.join(TEMPDIR, "demo_module.py")
            with open(demo_module_path, 'wt') as fp:
                fp.write(TEST_ASYNCIO)

            rc, out, err = assert_python_failure(*args)
            self.assertIn(
                b"AttributeError: module 'demo_module' has no attribute 'coroutine'",
                err
            )
        finally:
            support.rmtree(TEMPDIR)

def test_main(verbose=None):
    support.run_unittest(TestAsyncIO_CLI)

if __name__ == '__main__':
    test_main(verbose=True)