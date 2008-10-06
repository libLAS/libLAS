using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using LibLAS;


namespace NUnitTest
{
    [TestFixture]
    public class LASVariableLengthRecordTest
    {
        

        [SetUp]
        protected void SetUp()
        {

        }

       
        [Test]
        public void TestDefault()
        {
            LASVariableLengthRecord lvlr = new LASVariableLengthRecord();

            Assert.AreEqual(lvlr.RecordId,0);
            Assert.AreEqual(lvlr.RecordLength, 0);
            Assert.AreEqual(lvlr.Reserved, 0);
            Assert.AreEqual(lvlr.UserId, "");
            Assert.AreEqual(lvlr.Description, "");

        }

        [Test]
        public void Equals()
        {
           
           

        

        }
        

    }
}

