using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using LibLAS;


namespace NUnitTest
{
    [TestFixture]
    public class LASGuidTest
    {
        // LASPoint.

        [SetUp]
        protected void SetUp()
        {

        }

       
        [Test]
        public void ConstructionfromString()
        {
            LASGuid guid1 = new LASGuid("00000000-0000-0000-0000-000000000000");

            LASGuid guid2 = new LASGuid("3F2504E0-4F89-11D3-9A0C-0305E82C3301");


            Assert.AreNotSame(guid1, guid2);


            Assert.AreEqual(guid1.ToString(), "00000000-0000-0000-0000-000000000000");
            Assert.AreEqual(guid2.ToString().ToUpper(), "3F2504E0-4F89-11D3-9A0C-0305E82C3301");
            Assert.IsNotNull(guid1);
            Assert.IsNotNull(guid2);
        

        }

        [Test]
        public void Equals()
        {
           
            LASGuid guid1 = new LASGuid("3F2504E0-4F89-11D3-9A0C-0305E82C3301");
            LASGuid guid2 = new LASGuid("3F2504E0-4F89-11D3-9A0C-0305E82C3301");


            Assert.IsTrue(guid1.Equals(guid2));

        

        }
        

    }
}

